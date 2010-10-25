/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#include <cstdlib>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/FileCatalog.hpp>

#include <bdbtool/cmd/Import.hpp>

namespace po = boost::program_options;

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
    
    // try to get a command handler
    brfc::shared_ptr<brfc::tool::Command> cmd;
    if (pargs.size() >= 1) {
        std::string name = pargs.front();
        cmd = brfc::tool::Command::by_name(name);
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
        std::cout << "usage: bdbtool [--dsn=] [--help] CMD [args]" << std::endl;
        std::cout << std::endl;
        std::cout << common_optdesc << std::endl;
        std::cout << std::endl;

        if (cmd)
            cmd->help(std::cout);

        return 1;
    }

    brfc::shared_ptr<brfc::FileCatalog> fc;

    if (dsn.size() < 1) {
        std::cerr << "missing DSN" << std::endl;
        return 1;
    } else {
        try {
            fc.reset(new brfc::FileCatalog(dsn, "/tmp"));
        } catch (const brfc::brfc_error& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
     
    return cmd->execute(*fc, cmd_args);
}
