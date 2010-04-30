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

#include <brfc/Path.hpp>

#include "common.hpp"

namespace brfc {

TEST(Path_test, is_absolute) {
    EXPECT_TRUE(Path("/path").is_absolute());
    EXPECT_FALSE(Path("path").is_absolute());
}

TEST(Path_test, is_dir) {
    EXPECT_TRUE(Path("/").is_dir());
    EXPECT_FALSE(Path("/nxpath").is_dir());
}

TEST(Path_test, exists) {
    EXPECT_TRUE(Path("/").exists());
    EXPECT_FALSE(Path("/nxpath").exists());
}

TEST(Path_test, join) {
    Path p("path1");
    EXPECT_EQ("path1/path2", p.join("path2").string());
}

} // namespace brfc
