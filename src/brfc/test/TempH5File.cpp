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

#include <brfc/test/TempH5File.hpp>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {
namespace test {

TempH5File::TempH5File()
        : path_(strdup("/tmp/brfctest_XXXXXX"), &free) {
    int fd = mkstemp(path_.get());
    if (fd == -1)
        throw std::runtime_error("could not create temporary file");
    close(fd);
}

TempH5File::~TempH5File() {
    unlink();
}

void
TempH5File::unlink() {
    ::unlink(path_.get());
}



std::string
TempH5File::path() const {
    return std::string(path_.get());
}

void
TempH5File::copy(const std::string& dest) const {
    BoostFileSystem().copy_file(path(), dest);
}

void
TempH5File::write(const Oh5File& file) {
    Oh5HlFileWriter().write(file, path());
}

} // namespace test
} // namespace brfc
