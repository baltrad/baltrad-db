/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/buildconfig.h>

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assign/ptr_map_inserter.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/program_options.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

#include <brfc/tool/Benchmark.hpp>
#include <brfc/tool/Import.hpp>
#ifdef BDB_BUILD_BDBFS
    #include <brfc/tool/Mount.hpp>
#endif // BDB_BUILD_BDBFS
#include <brfc/tool/Show.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {

struct Commands {
    typedef boost::ptr_map<std::string, Command> CommandMap;

    Commands()
            : map_() {
        using boost::assign::ptr_map_insert;
        ptr_map_insert<Benchmark>(map_)("benchmark");
        ptr_map_insert<Import>(map_)("import");
#ifdef BDB_BUILD_BDBFS
        ptr_map_insert<Mount>(map_)("mount");
#endif // BDB_BUILD_BDBFS
        ptr_map_insert<Show>(map_)("show");
    }

    Command* by_name(const std::string& name) {
        CommandMap::iterator it = map_.find(name);
        if (it != map_.end())
            return it->second;
        return 0;
    }

    void print_list(std::ostream& out) {
        BOOST_FOREACH(const CommandMap::value_type& value, map_) {
            out << " " << std::setw(15) << std::left << value.first
                << value.second->description() << std::endl;
        }
    }
    
    CommandMap map_;
};

} // namespace tool
} // namespace brfc

int
main(int argc, char** argv) {
    // set up default DSN from env
    std::string dsn;
    const char* env_dsn = std::getenv("BDB_DSN");
    if (env_dsn) {
        dsn = std::string(env_dsn);
    }
    
    // common options
    po::options_description common_optdesc("common options");
    common_optdesc.add_options()
        ("help", "show this help message")
        ("dsn", po::value<std::string>(&dsn),
                "BDB database url. "
                "If missing, taken from environment variable 'BDB_DSN'")
    ;
    
    // positional arguments
    std::vector<std::string> pargs;
    po::options_description pargs_optdesc("");
    pargs_optdesc.add_options()
        ("pargs", po::value<std::vector<std::string> >(&pargs), "positional arguments")
    ;

    po::positional_options_description pargs_desc;
    pargs_desc.add("pargs", -1);
    
    po::options_description combined_optdesc("");
    combined_optdesc.add(common_optdesc).add(pargs_optdesc);
    
    po::variables_map vm;
    po::parsed_options opts =
        po::command_line_parser(argc, argv).options(combined_optdesc)
                                           .positional(pargs_desc)
                                           .allow_unregistered()
                                           .run();
    po::store(opts, vm);
    po::notify(vm);
    
    // collect unrecognized options as arguments to actual command
    std::vector<std::string> cmd_args =
        po::collect_unrecognized(opts.options, po::exclude_positional);
    
    brfc::tool::Commands commands;
    // try to get a command handler
    brfc::tool::Command* cmd = 0;
    if (pargs.size() >= 1) {
        std::string name = pargs.front();
        cmd = commands.by_name(name);
        if (not cmd) {
            std::cerr << "'" << name << "' "
                      << "is not a valid bdbtool command, see 'bdbtool --help'"
                      << std::endl;
            return 1;
        }
        pargs.erase(pargs.begin());
    }
    
    // add remaining positional arguments to command arguments
    cmd_args.insert(cmd_args.end(), pargs.begin(), pargs.end());
    
    // display help if requested or no command specified
    if (vm.count("help") or not cmd) {
        std::cout << "usage: bdbtool [--dsn=] [--help] CMD [args]"
                  << std::endl << std::endl
                  << common_optdesc << std::endl;

        if (cmd) {
            cmd->help(std::cout);
        } else {
            std::cout << "available commands: " << std::endl;
            commands.print_list(std::cout);
            std::cout << std::endl
                      << "use 'bdbtool --help CMD' for more detailed help"
                      << std::endl;
        }

        std::cout << std::endl;

        return 1;
    }
    
    // XXX: get from a factory
    boost::shared_ptr<brfc::Database> db;

    if (dsn.size() < 1) {
        std::cerr << "missing DSN" << std::endl;
        return 1;
    } else {
        try {
            db.reset(brfc::Database::create(dsn));
        } catch (const brfc::brfc_error& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    try {
        cmd->parse_args(cmd_args);
    } catch (const brfc::value_error& e) {
        std::cout << "bdbtool: " << e.what() << std::endl;
        return 1;
    }

    return cmd->execute(*db);
}
