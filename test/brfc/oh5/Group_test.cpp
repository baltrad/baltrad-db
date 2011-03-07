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

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/MemoryNodeBackend.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/test_common.hpp>

namespace brfc {
namespace oh5 {

struct oh5_Group_test : public ::testing::Test {
    oh5_Group_test()
            : g("g") {
        g.backend(new MemoryNodeBackend());
    }
    
    Group g;
};

TEST_F(oh5_Group_test, test_accepts_child_Attribute) {
    Attribute node("attr", Scalar(1));
    EXPECT_TRUE(g.accepts_child(node));
}

TEST_F(oh5_Group_test, test_accepts_child_Group) {
    Group node("group");
    EXPECT_TRUE(g.accepts_child(node));
}

TEST_F(oh5_Group_test, test_accepts_child_RootGroup) {
    RootGroup node;
    EXPECT_FALSE(g.accepts_child(node));
}

TEST_F(oh5_Group_test, test_accepts_child_DataSet) {
    DataSet node("data");
    EXPECT_TRUE(g.accepts_child(node));
}

TEST_F(oh5_Group_test, test_attribute_access) {
    Group& what = g.create_group("what");
    Attribute& a1 = what.create_attribute("a1", Scalar(1));

    EXPECT_EQ(&a1, g.attribute("what/a1"));
    EXPECT_FALSE(g.attribute("a1"));
    EXPECT_FALSE(g.attribute("what"));
}

TEST_F(oh5_Group_test, test_group_access) {
    Group& g2 = g.create_group("g2");
    Group& g3 = g2.create_group("g3");
    
    EXPECT_EQ(&g2, g.group("g2"));
    EXPECT_EQ(&g3, g2.group("g3"));
    EXPECT_FALSE(g.group("g3"));
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
TEST_F(oh5_Group_test, test_effective_attribute_access) {
    Scalar val(1);

    Group& w = g.create_group("what");
    Attribute& w_attr1 = w.create_attribute("attr1", val);
    Attribute& w_attr2 = w.create_attribute("attr2", val);
    (void)w_attr2;
    Group& ds1 = g.create_group("dataset1");
    Attribute& ds1_attr1 = ds1.create_attribute("attr1", val);
    Group& ds1_d1 = ds1.create_group("data1");
    Group& ds1_d1_w = ds1_d1.create_group("what");
    Attribute& ds1_d1_w_attr1 = ds1_d1_w.create_attribute("attr1", val);
    Attribute& ds1_d1_w_attr2 = ds1_d1_w.create_attribute("attr2", val);
    (void)ds1_d1_w_attr2;
    Group& ds1_d2 = ds1.create_group("data2");


    EXPECT_EQ(&ds1_attr1, ds1_d2.effective_attribute("attr1"));
    EXPECT_EQ(0, ds1_d2.effective_attribute("attr2"));
    EXPECT_EQ(&w_attr1, ds1_d2.effective_attribute("what/attr1"));
    EXPECT_EQ(&ds1_d1_w_attr1, ds1_d1.effective_attribute("what/attr1"));
}

TEST_F(oh5_Group_test, test_get_or_create_group_absolute_path_throws) {
    Group& g2 = g.create_group("g2");
    EXPECT_THROW(g2.get_or_create_group("/dataset1"), value_error);
    EXPECT_FALSE(g2.has_child("dataset1"));
}

TEST_F(oh5_Group_test, test_get_or_create_group) {
    Group& child1 = g.get_or_create_group("dataset1/data1");
    EXPECT_EQ("data1", child1.name());
    EXPECT_TRUE(g.has_child("dataset1/data1"));
    
    Group& child2 = g.get_or_create_group("dataset1/data1");

    EXPECT_EQ(&child1, &child2);
}

} // namespace oh5
} // namespace brfc
