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

#ifndef BRFC_FUSE_DAEMON_HPP
#define BRFC_FUSE_DAEMON_HPP

#include <string>
#include <vector>

#include <fuse.h>

namespace brfc {
namespace fuse {

class Entry;

class Daemon {
  public:
    static Daemon& instance();

    Entry& root();

    void root(Entry* root) {
        root_ = root;
    }

    int run(const std::vector<std::string>& args);

    static
    int
    getattr(const char* path, struct stat* stbuf);

    static
    int
    readdir(const char* path,
            void* buf,
            fuse_fill_dir_t filler,
            off_t offset,
            struct fuse_file_info* fi);
    
    static
    int
    open(const char* path, fuse_file_info* fi);

    static
    int
    read(const char* path,
         char* buf,
         size_t size,
         off_t offset,
         fuse_file_info* fi);

    static
    int
    release(const char* path, fuse_file_info* fi);

  private:
    Daemon();

    Entry* root_;
    fuse_operations ops_;
};


} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_DAEMON_HPP
