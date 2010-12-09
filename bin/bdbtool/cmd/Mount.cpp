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

#include <bdbtool/cmd/Mount.hpp>

#include <iostream>

#include <unistd.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <brfc/DefaultFileNamer.hpp>

#include <brfc/db/Database.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Literal.hpp>

#include <brfc/fuse/Daemon.hpp>
#include <brfc/fuse/FileFactory.hpp>
#include <brfc/fuse/DirEntry.hpp>
#include <brfc/fuse/DirFactory.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {
namespace cmd {

Mount::Mount()
        : optdesc_("mount options")
        , uid_(getuid())
        , gid_(getgid()) {
    optdesc_.add_options()
        ("mount-point", po::value<std::string>(),
            "directory where to mount the filesystem ")
        ("debug,d",
            "turn on debugging, implies -f")
        ("foreground,f",
            "do not detach")
        ("uid", po::value<uid_t>(&uid_),
            "user id (default: current user)")
        ("gid", po::value<gid_t>(&gid_),
            "group id (default: current group)")
    ;
}

std::string
Mount::do_description() const {
    return "mount bdbfs";
}

void
Mount::do_help(std::ostream& out) const {
    out << optdesc_;
}

int
Mount::do_execute(db::Database& db,
                  const std::vector<std::string>& args) {

    po::positional_options_description p_optdesc_;
    p_optdesc_.add("mount-point", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(args).
              options(optdesc_).positional(p_optdesc_).run(), vm);
    po::notify(vm);
    

    if (not vm.count("mount-point") == 1) {
        help(std::cout);
        return 1;
    }

    std::vector<std::string> dargs;
    dargs.push_back("bdbtool");
    dargs.push_back(vm["mount-point"].as<std::string>());
    if (vm.count("debug"))
        dargs.push_back("-d");
    if (vm.count("foreground"))
        dargs.push_back("-f");
    dargs.push_back("-o");
    dargs.push_back("uid=" + boost::lexical_cast<std::string>(uid_));
    dargs.push_back("-o");
    dargs.push_back("gid=" + boost::lexical_cast<std::string>(gid_));

    fuse::Daemon& daemon = fuse::Daemon::instance();
    expr::ExpressionFactory xpr;
    
    DefaultFileNamer namer;
    fuse::FileFactory l3fac(&db, &namer);

    fuse::DirFactory l2fac(&db,
                           *xpr.attribute("what/object"),
                           &l3fac);
    
    fuse::DirFactory l1fac(&db,
                           *xpr.attribute("what/source:_name"),
                           &l2fac); 

    fuse::DirEntry root("", l1fac);

    daemon.root(&root);
    return daemon.run(dargs);
}

} // namespace cmd
} // namespace tool
} // namespace brfc
