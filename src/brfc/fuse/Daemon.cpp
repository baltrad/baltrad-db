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

#include <brfc/fuse/Daemon.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>

#include <brfc/fuse/Entry.hpp>

namespace brfc {
namespace fuse {

Daemon::Daemon()
        : ops_() {
    ops_.getattr = Daemon::getattr;
    ops_.readdir = Daemon::readdir;
    ops_.open = Daemon::open;
    ops_.read = Daemon::read;
    ops_.release = Daemon::release;
}

Daemon&
Daemon::instance() {
    static Daemon d;
    return d;
}

Entry&
Daemon::root() {
    BRFC_ASSERT(root_ != 0);
    return *root_;
}

int
Daemon::run(const std::vector<std::string>& args) {
    struct fuse_args fargs = FUSE_ARGS_INIT(0, 0);
    BOOST_FOREACH(const std::string& arg, args) {
        fuse_opt_add_arg(&fargs, arg.c_str());
    }
    fuse_opt_add_arg(&fargs, "-o");
    fuse_opt_add_arg(&fargs, "ro");
    fuse_opt_add_arg(&fargs, "-o");
    fuse_opt_add_arg(&fargs, "fsname=bdbfs");
    return fuse_main(fargs.argc, fargs.argv, &ops_, 0);
}

int
Daemon::getattr(const char* path, struct stat* stbuf) {
    return instance().root().find(path).stat(*stbuf);
}

int
Daemon::readdir(const char* path,
                void* buf,
                fuse_fill_dir_t filler,
                off_t offset,
                fuse_file_info* fi) {
    return instance().root().find(path).readdir(buf, filler, offset, *fi);
}

int
Daemon::open(const char* path, struct fuse_file_info* fi) {
    return instance().root().find(path).open(*fi);
}

int
Daemon::read(const char* path,
             char* buf,
             size_t size,
             off_t offset,
             fuse_file_info* fi) {
    return instance().root().find(path).read(buf, size, offset, *fi);
}

int
Daemon::release(const char* path, fuse_file_info* fi) {
    return instance().root().find(path).close(*fi);
}


} // namespace fuse
} // namespace brfc
