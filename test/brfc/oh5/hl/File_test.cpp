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
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/oh5/hl/File.hpp>

#include <brfc/test/TempH5File.hpp>

#include "../../common.hpp"

namespace brfc {
namespace oh5 {
namespace hl {

struct oh5_hl_File_test : public testing::Test {
    oh5_hl_File_test() {

    }

    File file;
};

TEST_F(oh5_hl_File_test, test_open_nx_file) {
    EXPECT_THROW(File("/path/to/nxfile"), fs_error);
}

TEST_F(oh5_hl_File_test, test_load_from_filesystem) {
    Scalar t_12_05_01(Time(12, 5, 1));
    Scalar d_2000_01_02(Date(2000, 1, 2));

    file.root().create_child_attribute("date", d_2000_01_02);
    file.root().create_child_attribute("time", t_12_05_01);
    Group& what = file.root().create_child_group("what");
    what.create_child_attribute("date", d_2000_01_02);

    test::TempH5File tempfile;
    tempfile.write(file);

    File g(tempfile.path());
    EXPECT_EQ(g.path(), tempfile.path());
    Group& root = g.root();
    EXPECT_EQ((size_t)3, root.children().size());
    EXPECT_TRUE(root.has_child_by_name("date"));
    EXPECT_TRUE(root.has_child_by_name("time"));
    EXPECT_TRUE(root.has_child_by_name("what"));
    ASSERT_TRUE(root.child_attribute("date"));
    ASSERT_TRUE(root.child_attribute("time"));
    ASSERT_TRUE(g.group("/what"));
    ASSERT_TRUE(g.group("/what")->child_attribute("date"));
    EXPECT_EQ("20000102", root.child_attribute("date")->value().string());
    EXPECT_EQ("120501", root.child_attribute("time")->value().string());
    EXPECT_EQ("20000102", g.group("/what")->child_attribute("date")->value().string());
}

} // namespace hl
} // namespace oh5
} // namespace brfc
