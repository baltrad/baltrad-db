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

#include <gtest/gtest.h>

#include <brfc/util/BoostFileSystem.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

namespace brfc {

class NamedTemporaryFile_test : public ::testing::Test {
  public:
    NamedTemporaryFile_test()
            : fs() {
    }

    BoostFileSystem fs;
};


TEST_F(NamedTemporaryFile_test, test_ctor) {
    NamedTemporaryFile f;

    EXPECT_NE("", f.path());
    EXPECT_TRUE(fs.exists(f.path()));
}

TEST_F(NamedTemporaryFile_test, test_dtor) {
    std::string path;
    {
        NamedTemporaryFile f;
        path = f.path();
    }

    EXPECT_FALSE(fs.exists(path));
}

TEST_F(NamedTemporaryFile_test, test_remove) {
    NamedTemporaryFile f;

    f.remove();
    
    EXPECT_EQ("", f.path());
    EXPECT_FALSE(fs.exists(f.path()));
}

TEST_F(NamedTemporaryFile_test, test_path) {
    NamedTemporaryFile f;

    EXPECT_TRUE(fs.is_absolute(f.path()));
}

} // namespace brfc
