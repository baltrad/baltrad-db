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
            : g(make_shared<Group>("g")) {
    }

    shared_ptr<Group> g;
};

TEST_F(oh5_Group_test, test_add_child_attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Scalar(1));
    EXPECT_NO_THROW(g->add_child(a));
}

TEST_F(oh5_Group_test, test_add_child_attributegroup) {
    shared_ptr<AttributeGroup> g2 = make_shared<AttributeGroup>("g2");
    EXPECT_NO_THROW(g->add_child(g2));
}

TEST_F(oh5_Group_test, test_add_child_group) {
    shared_ptr<Group> d = make_shared<Group>("d");
    EXPECT_NO_THROW(g->add_child(d));
}

TEST_F(oh5_Group_test, test_child_attribute_access) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    g->add_child(what);
    shared_ptr<Attribute> a1 = make_shared<Attribute>("a1", Scalar(1));
    what->add_child(a1);

    EXPECT_EQ(g->child_attribute("what/a1"), a1);
    EXPECT_FALSE(g->child_attribute("a1"));
    EXPECT_FALSE(g->child_attribute("what"));
}

TEST_F(oh5_Group_test, test_child_group_by_name) {
    shared_ptr<Group> g2 = make_shared<Group>("g2");
    g->add_child(g2);
    shared_ptr<AttributeGroup> ag = make_shared<AttributeGroup>("ag");
    g2->add_child(ag);
    
    EXPECT_EQ(g->child_group_by_name("g2"), g2);
    EXPECT_EQ(g2->child_group_by_name("ag"), ag);
    EXPECT_FALSE(g->child_group_by_name("ag"));
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
    shared_ptr<AttributeGroup> w = make_shared<AttributeGroup>("what");
    shared_ptr<AttributeGroup> ds1_d1_w = make_shared<AttributeGroup>("what");

    shared_ptr<Group> ds1 = make_shared<Group>("dataset1");
    shared_ptr<Group> ds1_d1 = make_shared<Group>("data1");
    shared_ptr<Group> ds1_d2 = make_shared<Group>("data2");

    Scalar val(1);
    shared_ptr<Attribute> w_attr1 = make_shared<Attribute>("attr1", val);
    shared_ptr<Attribute> w_attr2 = make_shared<Attribute>("attr2", val);
    shared_ptr<Attribute> ds1_attr1 = make_shared<Attribute>("attr1", val);
    shared_ptr<Attribute> ds1_d1_w_attr1 = make_shared<Attribute>("attr1", val);
    shared_ptr<Attribute> ds1_d1_w_attr2 = make_shared<Attribute>("attr2", val);

    g->add_child(w);
    g->add_child(ds1);
    w->add_child(w_attr1);
    w->add_child(w_attr2);
    ds1->add_child(ds1_d1);
    ds1->add_child(ds1_d2);
    ds1->add_child(ds1_attr1);
    ds1_d1->add_child(ds1_d1_w);
    ds1_d1_w->add_child(ds1_d1_w_attr1);
    ds1_d1_w->add_child(ds1_d1_w_attr2);

    EXPECT_EQ(ds1_attr1, ds1_d2->attribute("attr1"));
    EXPECT_EQ(shared_ptr<Attribute>(), ds1_d2->attribute("attr2"));
    EXPECT_EQ(w_attr1, ds1_d2->attribute("what/attr1"));
    EXPECT_EQ(ds1_d1_w_attr1, ds1_d1->attribute("what/attr1"));
}

TEST_F(oh5_Group_test, test_create_by_name_valid_names) {
    shared_ptr<Group> grp;

    grp = Group::create_by_name("dataset1");
    EXPECT_TRUE(dynamic_pointer_cast<Group>(grp));

    grp = Group::create_by_name("data2");
    EXPECT_TRUE(dynamic_pointer_cast<Group>(grp));

    grp = Group::create_by_name("quality3");
    EXPECT_TRUE(dynamic_pointer_cast<Group>(grp));

    grp = Group::create_by_name("what");
    EXPECT_TRUE(dynamic_pointer_cast<AttributeGroup>(grp));
    grp = Group::create_by_name("where");
    EXPECT_TRUE(dynamic_pointer_cast<AttributeGroup>(grp));
    grp = Group::create_by_name("how");
    EXPECT_TRUE(dynamic_pointer_cast<AttributeGroup>(grp));
}

TEST_F(oh5_Group_test, test_create_by_name_invalid_names) {
    EXPECT_FALSE(Group::create_by_name("dataset1a2"));
    EXPECT_FALSE(Group::create_by_name("data"));
    EXPECT_FALSE(Group::create_by_name("what/bla"));
    EXPECT_FALSE(Group::create_by_name("/dataset1"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_name_invalid) {
    shared_ptr<Group> child;

    child = g->get_or_create_child_group_by_name("qwe");
    EXPECT_FALSE(child);
    EXPECT_FALSE(g->has_child_by_name("qwe"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_name_valid) {
    shared_ptr<Group> child1, child2;

    child1 = g->get_or_create_child_group_by_name("dataset1");
    EXPECT_TRUE(child1);
    EXPECT_TRUE(g->has_child_by_name("dataset1"));

    child2 = g->get_or_create_child_group_by_name("dataset1");
    EXPECT_TRUE(child2);

    EXPECT_EQ(child1, child2);
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path_invalid) {
    shared_ptr<Group> child;

    StringList path = String("path/to").split("/");

    child = g->get_or_create_child_group_by_path(path);
    EXPECT_FALSE(child);
    EXPECT_FALSE(g->has_child_by_name("path"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path_valid) {
    shared_ptr<Group> child1, child2;

    StringList path = String("dataset1/data1").split("/");

    child1 = g->get_or_create_child_group_by_path(path);
    EXPECT_TRUE(child1);
    EXPECT_EQ("data1", child1->name());
    ASSERT_TRUE(g->has_child_by_name("dataset1"));
    EXPECT_TRUE(g->child_by_name("dataset1")->has_child_by_name("data1"));
    
    child2 = g->get_or_create_child_group_by_path(path);
    EXPECT_TRUE(child2);

    EXPECT_EQ(child1, child2);
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path_invalid_end) {
    shared_ptr<Group> child;

    StringList path = String("dataset1/invalid").split("/");
    
    child = g->get_or_create_child_group_by_path(path);
    EXPECT_FALSE(child);
    EXPECT_FALSE(g->has_child_by_name("dataset1"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_grop_by_path_invalid_end2) {
    shared_ptr<Group> child, ds1;

    StringList path = String("dataset1/invalid").split("/");
    ds1 = g->get_or_create_child_group_by_name("dataset1");
    child = g->get_or_create_child_group_by_path(path);
    EXPECT_FALSE(child);
    EXPECT_TRUE(g->has_child_by_name("dataset1"));
    EXPECT_FALSE(ds1->has_child_by_name("data1"));
}

TEST_F(oh5_Group_test, test_get_or_create_child_group_by_path_unaccepted) {
    shared_ptr<Group> child;
    StringList path = String("dataset1/what/quality1").split("/");
    
    EXPECT_THROW(child = g->get_or_create_child_group_by_path(path), value_error);
    EXPECT_FALSE(g->has_child_by_name("dataset1"));
}


} // namespace oh5
} // namespace brfc
