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

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5MemoryNodeBackend.hpp>
#include <brfc/oh5/Oh5Group.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

struct oh5_Oh5Group_test : public ::testing::Test {
    oh5_Oh5Group_test()
            : backend()
            , g(static_cast<Oh5Group&>(backend.root().add(new Oh5Group("g")))) {

    }
    
    Oh5MemoryNodeBackend backend;
    Oh5Group& g;
};

TEST_F(oh5_Oh5Group_test, test_accepts_child_Attribute) {
    Oh5Attribute node("attr", Oh5Scalar(1));
    EXPECT_TRUE(g.accepts_child(node));
}

TEST_F(oh5_Oh5Group_test, test_accepts_child_Group) {
    Oh5Group node("group");
    EXPECT_TRUE(g.accepts_child(node));
}

TEST_F(oh5_Oh5Group_test, test_accepts_child_DataSet) {
    Oh5DataSet node("data");
    EXPECT_TRUE(g.accepts_child(node));
}

// XXX: implemented in Node
TEST_F(oh5_Oh5Group_test, test_attribute_access) {
    Oh5Group& what = static_cast<Oh5Group&>(g.add(new Oh5Group("what")));
    Oh5Attribute& a1 = static_cast<Oh5Attribute&>(what.add(new Oh5Attribute("a1", Oh5Scalar(1))));

    EXPECT_EQ(&a1, g.attribute("what/a1"));
    EXPECT_FALSE(g.attribute("a1"));
    EXPECT_FALSE(g.attribute("what"));
}

// XXX: implemented in Node
TEST_F(oh5_Oh5Group_test, test_group_access) {
    Oh5Group& g2 = static_cast<Oh5Group&>(g.add(new Oh5Group("g2")));
    Oh5Group& g3 = static_cast<Oh5Group&>(g2.add(new Oh5Group("g3")));
    
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
TEST_F(oh5_Oh5Group_test, test_effective_attribute_access) {
    Oh5Scalar val(1);

    Oh5Group& w = static_cast<Oh5Group&>(g.add(new Oh5Group("what")));
    Oh5Attribute& w_attr1 = static_cast<Oh5Attribute&>(w.add(new Oh5Attribute("attr1", val)));
    Oh5Attribute& w_attr2 = static_cast<Oh5Attribute&>(w.add(new Oh5Attribute("attr2", val)));
    (void)w_attr2;
    Oh5Group& ds1 = static_cast<Oh5Group&>(g.add(new Oh5Group("dataset1")));
    Oh5Attribute& ds1_attr1 = static_cast<Oh5Attribute&>(ds1.add(new Oh5Attribute("attr1", val)));
    Oh5Group& ds1_d1 = static_cast<Oh5Group&>(ds1.add(new Oh5Group("data1")));
    Oh5Group& ds1_d1_w = static_cast<Oh5Group&>(ds1_d1.add(new Oh5Group("what")));
    Oh5Attribute& ds1_d1_w_attr1 = static_cast<Oh5Attribute&>(ds1_d1_w.add(new Oh5Attribute("attr1", val)));
    Oh5Attribute& ds1_d1_w_attr2 = static_cast<Oh5Attribute&>(ds1_d1_w.add(new Oh5Attribute("attr2", val)));
    (void)ds1_d1_w_attr2;
    Oh5Group& ds1_d2 = static_cast<Oh5Group&>(ds1.add(new Oh5Group("data2")));


    EXPECT_EQ(&ds1_attr1, ds1_d2.effective_attribute("attr1"));
    EXPECT_EQ(0, ds1_d2.effective_attribute("attr2"));
    EXPECT_EQ(&w_attr1, ds1_d2.effective_attribute("what/attr1"));
    EXPECT_EQ(&ds1_d1_w_attr1, ds1_d1.effective_attribute("what/attr1"));
}

TEST_F(oh5_Oh5Group_test, test_get_or_create_group_absolute_path_throws) {
    Oh5Group& g2 = static_cast<Oh5Group&>(g.add(new Oh5Group("g2")));
    EXPECT_THROW(g2.get_or_create_group("/dataset1"), std::invalid_argument);
    EXPECT_FALSE(g2.has_child("dataset1"));
}

TEST_F(oh5_Oh5Group_test, test_get_or_create_group) {
    Oh5Group& child1 = g.get_or_create_group("dataset1/data1");
    EXPECT_EQ("data1", child1.name());
    EXPECT_TRUE(g.has_child("dataset1/data1"));
    
    Oh5Group& child2 = g.get_or_create_group("dataset1/data1");

    EXPECT_EQ(&child1, &child2);
}

} // namespace brfc
