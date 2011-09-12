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

#include <brfc/util/NamedTemporaryFile.hpp>

#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace brfc {

NamedTemporaryFile::NamedTemporaryFile()
        : path_(strdup("/tmp/brfctest_XXXXXX"), &free) {
    int fd = mkstemp(path_.get());
    if (fd == -1)
        throw std::runtime_error("could not create temporary file");
    close(fd);
}

NamedTemporaryFile::~NamedTemporaryFile() {
    remove();
}

void
NamedTemporaryFile::remove() {
    if (path_) {
        ::unlink(path_.get());
    }
    path_.reset();
}

std::string
NamedTemporaryFile::path() {
    if (path_)
        return std::string(path_.get());
    else
        return "";
}

} // namespace brfc
