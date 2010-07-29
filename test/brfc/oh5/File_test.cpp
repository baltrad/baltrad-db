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
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_File_test : public testing::Test {
    oh5_File_test()
            : f1(File::minimal("pvol",
                               Date(2000, 1, 2),
                               Time(12, 5),
                               "WMO:02606")) {
    }

    shared_ptr<File> f1;
};

TEST_F(oh5_File_test, get_nx_node) {
    shared_ptr<File> f = File::create();
    EXPECT_FALSE(f->group("/nx"));
}

TEST_F(oh5_File_test, root) {
    shared_ptr<File> f = File::create();
    EXPECT_EQ(f->root(), f->group("/"));
    EXPECT_EQ(f, f->root()->file());
}

TEST_F(oh5_File_test, required_attribute_shortcuts) {
    EXPECT_EQ(f1->what_object(), "pvol");
    EXPECT_EQ(f1->what_date(), Date(2000, 1, 2));
    EXPECT_EQ(f1->what_time(), Time(12, 5));
    EXPECT_EQ(f1->what_source(), "WMO:02606");
}

TEST_F(oh5_File_test, required_attribute_shortcuts_when_missing) {
    shared_ptr<File> f = File::create();
    EXPECT_THROW(f->what_object(), lookup_error);
    EXPECT_THROW(f->what_date(), lookup_error);
    EXPECT_THROW(f->what_time(), lookup_error);
    EXPECT_THROW(f->what_source(), lookup_error);
}

TEST_F(oh5_File_test, required_attribute_shortcuts_conversion) {
    shared_ptr<File> f = File::create();
    shared_ptr<Group> what = f->root()->get_or_create_child_group_by_name("what");
    what->add_child(make_shared<Attribute>("date", Variant("20001213")));
    what->add_child(make_shared<Attribute>("time", Variant("123456")));
    EXPECT_EQ(Date(2000, 12, 13), f->what_date());
    EXPECT_EQ(Time(12, 34, 56), f->what_time());

    what->child_attribute("date")->value(Variant("foo"));
    EXPECT_THROW(f->what_date(), value_error);

    what->child_attribute("time")->value(Variant("bar"));
    EXPECT_THROW(f->what_time(), value_error);
}

TEST_F(oh5_File_test, open_nx_file) {
    EXPECT_THROW(File::from_filesystem("/path/to/nxfile"), fs_error);
}

TEST_F(oh5_File_test, test_name) {
    EXPECT_EQ(f1->name(), "");
    f1->path("/path/to/filename");
    EXPECT_EQ(f1->name(), "filename");
    f1->path("filename2");
    EXPECT_EQ(f1->name(), "filename2");
}

TEST_F(oh5_File_test, test_invalid_attributes) {
    shared_ptr<Attribute> invalid = make_shared<Attribute>("invalid");
    f1->root()->add_child(invalid);
    File::AttributeVector v = f1->invalid_attributes();
    EXPECT_EQ(v.size(), (size_t)1);
    EXPECT_TRUE(std::find(v.begin(), v.end(), invalid) != v.end());
}

TEST_F(oh5_File_test, test_invalid_attribute_paths) {
    shared_ptr<Attribute> invalid = make_shared<Attribute>("invalid");
    f1->root()->add_child(invalid);
    const StringList& v = f1->invalid_attribute_paths();
    EXPECT_EQ(v.size(), (size_t)1);
    EXPECT_TRUE(v.contains("/invalid"));
}

} // namespace oh5
} // namespace brfc
