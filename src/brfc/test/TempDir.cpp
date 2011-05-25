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

#include <brfc/test/TempDir.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

namespace brfc {
namespace test {

TempDir::TempDir()
        : path_(strdup("/tmp/brfctest_XXXXXX"), &free) {
    if (mkdtemp(path_.get()) == 0) {
        throw std::runtime_error("could not create temporary directory");
    }
}

TempDir::~TempDir() {
    unlink();
}

void
TempDir::unlink() {
    rmdir_recurse(path_.get());
}

std::string
TempDir::path() const {
    return std::string(path_.get());
}

void
TempDir::rmdir_recurse(const char* path) {
    DIR* dir = opendir(path);
    if (dir == 0)
        return;

    struct dirent* ent = 0;
    for (ent = readdir(dir); ent != 0; ent = readdir(dir)) {
        if (strcmp(ent->d_name, ".") == 0 or strcmp(ent->d_name, "..") == 0)
            continue;
        std::string full_path = std::string(path) + "/" + ent->d_name;
        if (ent->d_type == DT_DIR) {
            rmdir_recurse(full_path.c_str());
        } else {
            if (::unlink(full_path.c_str()) == -1)
                std::cerr << "could not unlink " << full_path
                          << ": " << strerror(errno) << std::endl;
        }
    }
    closedir(dir);
    if (rmdir(path) == -1)
        std::cerr << "could not unlink " << path
                  << ": " << strerror(errno) << std::endl;
}

} // namespace test
} // namespace brfc
