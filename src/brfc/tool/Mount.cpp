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

#include <brfc/tool/Mount.hpp>

#include <iostream>

#include <unistd.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DefaultFileNamer.hpp>

#include <brfc/db/Database.hpp>

#include <brfc/expr/ExpressionFactory.hpp>

#include <brfc/fuse/Daemon.hpp>
#include <brfc/fuse/FileFactory.hpp>
#include <brfc/fuse/DirEntry.hpp>
#include <brfc/fuse/DirFactory.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {

Mount::Mount()
        : optdesc_("mount options")
        , mount_point_()
        , debug_(false)
        , foreground_(false)
        , uid_(getuid())
        , gid_(getgid()) {
    optdesc_.add_options()
        ("mount-point", po::value<std::string>(),
            "directory where to mount the filesystem")
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

void
Mount::do_parse_args(const ArgVector& args) {
    po::positional_options_description p_optdesc_;
    p_optdesc_.add("mount-point", 1);

    po::variables_map vm;

    try {
        po::store(po::command_line_parser(args).
                  options(optdesc_).positional(p_optdesc_).run(), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        throw value_error(e.what());
    }

    if (not vm.count("mount-point") == 1)
        throw value_error("missing mount-point");

    mount_point_ = vm["mount-point"].as<std::string>();

    debug_ = vm.count("debug") > 0;
    foreground_ = vm.count("foreground") > 0;
}

int
Mount::do_execute(Database& db) {
    std::vector<std::string> dargs;
    dargs.push_back("bdbtool");
    dargs.push_back(mount_point_);
    if (debug_)
        dargs.push_back("-d");
    if (foreground_)
        dargs.push_back("-f");
    dargs.push_back("-o");
    dargs.push_back("uid=" + boost::lexical_cast<std::string>(uid_));
    dargs.push_back("-o");
    dargs.push_back("gid=" + boost::lexical_cast<std::string>(gid_));

    fuse::Daemon& daemon = fuse::Daemon::instance();
    ExpressionFactory xpr;
    
    DefaultFileNamer namer;
    fuse::FileFactory l4fac(&db, &namer);

    fuse::DirFactory l3fac(&db,
                           xpr.attribute("what/date"),
                           &l4fac);

    fuse::DirFactory l2fac(&db,
                           xpr.attribute("what/object"),
                           &l3fac);
    
    fuse::DirFactory l1fac(&db,
                           xpr.attribute("what/source:_name"),
                           &l2fac); 

    fuse::DirEntry root("", l1fac);

    daemon.root(&root);
    return daemon.run(dargs);
}

} // namespace tool
} // namespace brfc
