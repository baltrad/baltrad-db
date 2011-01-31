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

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {

class util_BoostFileSystem_test : public ::testing::Test {
  public:
    util_BoostFileSystem_test()
            : fs() {
    }
  
    BoostFileSystem fs;
};

TEST_F(util_BoostFileSystem_test, file_size_nxpath) {
    EXPECT_THROW(fs.file_size("/path/to/nxfile"), fs_error);
}

TEST_F(util_BoostFileSystem_test, test_copy_file_nxsrc) {
    EXPECT_THROW(fs.copy_file("/path/to/nxfile", "/tmp/dst"), fs_error);
}

TEST_F(util_BoostFileSystem_test, test_clear_directory_nxfile) {
    EXPECT_THROW(fs.clear_directory("/path/to/nxfile"), fs_error);
}

} // namespace brfc
