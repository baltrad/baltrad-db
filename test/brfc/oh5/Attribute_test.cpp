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
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/QualityGroup.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Attribute_test : public ::testing::Test {
  public:
    oh5_Attribute_test()
            : a1(make_shared<Attribute>("a1", Variant(1)))
            , a2(make_shared<Attribute>("a2", Variant(2)))
            , what(make_shared<AttributeGroup>("what"))
            , dataset1(make_shared<DataSetGroup>("dataset1"))
            , root(make_shared<RootGroup>()) {

    }
    
    shared_ptr<Attribute> a1;
    shared_ptr<Attribute> a2;
    shared_ptr<AttributeGroup> what;
    shared_ptr<DataSetGroup> dataset1;
    shared_ptr<RootGroup> root;
};

TEST_F(oh5_Attribute_test, test_is_valid) {
    shared_ptr<Attribute> empty = make_shared<Attribute>("empty");
    EXPECT_FALSE(empty->is_valid());
    empty->value(Variant(1));
    EXPECT_TRUE(empty->is_valid());
}

TEST_F(oh5_Attribute_test, test_full_name) {
    EXPECT_EQ(a1->full_name(), "a1");
    root->add_child(a1);
    EXPECT_EQ(a1->full_name(), "a1");
    what->add_child(a2);
    EXPECT_EQ(a2->full_name(), "what/a2");
}

TEST_F(oh5_Attribute_test, to_string) {
    EXPECT_EQ(a1->to_string(), "/a1=1");
    root->add_child(dataset1);
    dataset1->add_child(what);
    what->add_child(a2);
    EXPECT_EQ(a2->to_string(), "/dataset1/what/a2=2");
}

TEST_F(oh5_Attribute_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_THROW(a1->add_child(a), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    EXPECT_THROW(a1->add_child(what), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_DataGroup) {
    shared_ptr<DataGroup> data1 = make_shared<DataGroup>("data1");
    EXPECT_THROW(a1->add_child(data1), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_DataSetGroup) {
    shared_ptr<DataSetGroup> dataset2 = make_shared<DataSetGroup>("dataset2");
    EXPECT_THROW(a1->add_child(dataset2), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_QualityGroup) {
    shared_ptr<QualityGroup> a1 = make_shared<QualityGroup>("a1");
    EXPECT_THROW(a1->add_child(a1), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_RootGroup) {
    shared_ptr<RootGroup> root = make_shared<RootGroup>();
    EXPECT_THROW(a1->add_child(root), value_error);
}

} // namepsace oh5
} // namespace brfc
