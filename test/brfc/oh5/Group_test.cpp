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
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Group_test : public ::testing::Test {
    oh5_Group_test()
            : g(0, "g") {
    }

    Group g;
};

TEST_F(oh5_Group_test, test_add_child_attribute) {
    auto_ptr<Node> a(new Attribute(0, "a", Scalar(1)));
    EXPECT_NO_THROW(g.add_child(a));
}

TEST_F(oh5_Group_test, test_add_child_attributegroup) {
    auto_ptr<Node> g2(new AttributeGroup(0, "g2"));
    EXPECT_NO_THROW(g.add_child(g2));
}

TEST_F(oh5_Group_test, test_add_child_group) {
    auto_ptr<Node> d(new Group(0, "d"));
    EXPECT_NO_THROW(g.add_child(d));
}

TEST_F(oh5_Group_test, test_child_attribute_access) {
    Group& what = g.create_child_group("what");
    Attribute& a1 = what.create_child_attribute("a1", Scalar(1));

    EXPECT_EQ(&a1, g.child_attribute("what/a1"));
    EXPECT_FALSE(g.child_attribute("a1"));
    EXPECT_FALSE(g.child_attribute("what"));
}

TEST_F(oh5_Group_test, test_child_group_by_name) {
    Group& g2 = g.create_child_group("g2");
    Group& g3 = g2.create_child_group("g3");
    
    EXPECT_EQ(&g2, g.child_group_by_name("g2"));
    EXPECT_EQ(&g3, g2.child_group_by_name("g3"));
    EXPECT_FALSE(g.child_group_by_name("g3"));
}

/*
 - g
   - what
     - attr1
     - attr2
   - dataset1
     - attr1
     - data1
       - what
         - attr1
         - attr2
     - data2
*/
TEST_F(oh5_Group_test, test_attribute_access) {
    Scalar val(1);

    Group& w = g.create_child_group("what");
    Attribute& w_attr1 = w.create_child_attribute("attr1", val);
    Attribute& w_attr2 = w.create_child_attribute("attr2", val);
    Group& ds1 = g.create_child_group("dataset1");
    Attribute& ds1_attr1 = ds1.create_child_attribute("attr1", val);
    Group& ds1_d1 = ds1.create_child_group("data1");
    Group& ds1_d1_w = ds1_d1.create_child_group("what");
    Attribute& ds1_d1_w_attr1 = ds1_d1_w.create_child_attribute("attr1", val);
    Attribute& ds1_d1_w_attr2 = ds1_d1_w.create_child_attribute("attr2", val);
    Group& ds1_d2 = ds1.create_child_group("data2");


    EXPECT_EQ(&ds1_attr1, ds1_d2.attribute("attr1"));
    EXPECT_EQ(0, ds1_d2.attribute("attr2"));
    EXPECT_EQ(&w_attr1, ds1_d2.attribute("what/attr1"));
    EXPECT_EQ(&ds1_d1_w_attr1, ds1_d1.attribute("what/attr1"));
}

TEST_F(oh5_Group_test, test_create_by_name_valid_names) {
    auto_ptr<Group> grp;

    grp = Group::create_by_name("dataset1");
    EXPECT_TRUE(grp.get());
    EXPECT_EQ(0, dynamic_cast<AttributeGroup*>(grp.get()));

    grp = Group::create_by_name("data2");
    EXPECT_TRUE(grp.get());
    EXPECT_EQ(0, dynamic_cast<AttributeGroup*>(grp.get()));

    grp = Group::create_by_name("quality3");
    EXPECT_TRUE(grp.get());
    EXPECT_EQ(0, dynamic_cast<AttributeGroup*>(grp.get()));

    grp = Group::create_by_name("what");
    EXPECT_TRUE(dynamic_cast<AttributeGroup*>(grp.get()));
    grp = Group::create_by_name("where");
    EXPECT_TRUE(dynamic_cast<AttributeGroup*>(grp.get()));
    grp = Group::create_by_name("how");
    EXPECT_TRUE(dynamic_cast<AttributeGroup*>(grp.get()));
}

TEST_F(oh5_Group_test, test_create_by_name_invalid_names) {
    EXPECT_THROW(Group::create_by_name("what/bla"), value_error);
    EXPECT_THROW(Group::create_by_name("/dataset1"), value_error);
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_name_invalid) {
    EXPECT_THROW(g.get_or_create_child_group_by_name("/dataset1"), value_error);
    EXPECT_FALSE(g.has_child_by_name("dataset1"));
    EXPECT_FALSE(g.has_child_by_name("/dataset1"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_name_valid) {
    Group& child1 = g.get_or_create_child_group_by_name("dataset1");
    EXPECT_TRUE(g.has_child_by_name("dataset1"));

    Group& child2 = g.get_or_create_child_group_by_name("dataset1");
    EXPECT_EQ(&child1, &child2);
}


TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path) {
    StringList path = String("dataset1/data1").split("/");

    Group& child1 = g.get_or_create_child_group_by_path(path);
    EXPECT_EQ("data1", child1.name());
    ASSERT_TRUE(g.has_child_by_name("dataset1"));
    EXPECT_TRUE(g.child_by_name("dataset1")->has_child_by_name("data1"));
    
    Group& child2 = g.get_or_create_child_group_by_path(path);

    EXPECT_EQ(&child1, &child2);
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path_unaccepted) {
    StringList path = String("dataset1/what/quality1").split("/");
    
    EXPECT_THROW(g.get_or_create_child_group_by_path(path), value_error);
    EXPECT_FALSE(g.has_child_by_name("dataset1"));
}


} // namespace oh5
} // namespace brfc
