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

#include <brfc/oh5/Oh5File.hpp>

namespace brfc {

struct oh5_Oh5File_test : public testing::Test {
    oh5_Oh5File_test()
            : file() {
    }
    
    Oh5File file;
};

TEST_F(oh5_Oh5File_test, test_name) {
    file.path("");
    EXPECT_EQ(file.name(), "");

    file.path("/path/to/filename");
    EXPECT_EQ(file.name(), "filename");

    file.path("filename2");
    EXPECT_EQ(file.name(), "filename2");
}


} // namespace brfc
