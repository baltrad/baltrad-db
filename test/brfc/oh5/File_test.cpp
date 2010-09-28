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

#include <algorithm>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/StringList.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/test/TempH5File.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_File_test : public testing::Test {
    oh5_File_test()
            : f1("pvol",
                 Date(2000, 1, 2),
                 Time(12, 5),
                 "WMO:02606")
            , f2() {
    }

    File f1, f2;
};

TEST_F(oh5_File_test, get_nx_node) {
    EXPECT_FALSE(f2.group("/nx"));
}

TEST_F(oh5_File_test, root) {
    EXPECT_EQ(&f2.root(), f2.group("/"));
    EXPECT_EQ(&f2, f2.root().file());
}

TEST_F(oh5_File_test, required_attribute_shortcuts) {
    EXPECT_EQ(f1.what_object(), "pvol");
    EXPECT_EQ(f1.what_date(), Date(2000, 1, 2));
    EXPECT_EQ(f1.what_time(), Time(12, 5));
    EXPECT_EQ(f1.what_source(), "WMO:02606");
}

TEST_F(oh5_File_test, required_attribute_shortcuts_when_missing) {
    EXPECT_THROW(f2.what_object(), lookup_error);
    EXPECT_THROW(f2.what_date(), lookup_error);
    EXPECT_THROW(f2.what_time(), lookup_error);
    EXPECT_THROW(f2.what_source(), lookup_error);
}

TEST_F(oh5_File_test, required_attribute_shortcuts_conversion) {
    Group& what = f2.root().get_or_create_child_group_by_name("what");
    what.create_child_attribute("date", Scalar("20001213"));
    what.create_child_attribute("time", Scalar("123456"));
    EXPECT_EQ(Date(2000, 12, 13), f2.what_date());
    EXPECT_EQ(Time(12, 34, 56), f2.what_time());

    what.child_attribute("date")->value(Scalar("foo"));
    EXPECT_THROW(f2.what_date(), value_error);

    what.child_attribute("time")->value(Scalar("bar"));
    EXPECT_THROW(f2.what_time(), value_error);
}

TEST_F(oh5_File_test, open_nx_file) {
    EXPECT_THROW(File("/path/to/nxfile"), fs_error);
}

TEST_F(oh5_File_test, test_name) {
    EXPECT_EQ(f1.name(), "");
    f1.path("/path/to/filename");
    EXPECT_EQ(f1.name(), "filename");
    f1.path("filename2");
    EXPECT_EQ(f1.name(), "filename2");
}

TEST_F(oh5_File_test, test_source_get) {
    EXPECT_EQ("", f2.source().to_string());

    EXPECT_EQ("WMO:02606", f1.source().to_string());
}

TEST_F(oh5_File_test, test_source_set) {
    Source s;
    s.add("WMO", "02606");
    s.add("RAD", "SE50");
    const String& expected = s.to_string();

    f1.source(s);
    EXPECT_EQ(expected, f1.what_source());

    f2.source(s);
    ASSERT_NO_THROW(f2.what_source());
    EXPECT_EQ(expected, f2.what_source());
}


TEST_F(oh5_File_test, test_load_from_filesystem) {
    Scalar t_12_05_01(Time(12, 5, 1));
    Scalar d_2000_01_02(Date(2000, 1, 2));

    f2.root().create_child_attribute("date", d_2000_01_02);
    f2.root().create_child_attribute("time", t_12_05_01);
    Group& what = f2.root().create_child_group("what");
    what.create_child_attribute("date", d_2000_01_02);

    test::TempH5File tempfile;
    tempfile.write(f2);

    File g(tempfile.path());
    EXPECT_EQ(g.path(), tempfile.path());
    RootGroup& root = g.root();
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


} // namespace oh5
} // namespace brfc
