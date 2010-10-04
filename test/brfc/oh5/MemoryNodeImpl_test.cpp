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

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/MemoryNodeImpl.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Scalar.hpp>

namespace brfc {
namespace oh5 {

class oh5_MemoryNodeImpl_test : public ::testing::Test {
  public:
    oh5_MemoryNodeImpl_test()
            : root(new MemoryNodeImpl("")) {

    }
  
  RootGroup root;
};

TEST_F(oh5_MemoryNodeImpl_test, test_root_create_Attribute) {
    Attribute* node = 0;
    EXPECT_NO_THROW(node = &root.create_attribute("attr", Scalar(1)));
    EXPECT_TRUE(node);
    EXPECT_THROW(root.create_attribute("attr", Scalar(1)), duplicate_entry);
    EXPECT_THROW(root.create_group("attr"), duplicate_entry);
}

TEST_F(oh5_MemoryNodeImpl_test, test_create_group) {
    Group* node = 0;
    EXPECT_NO_THROW(node = &root.create_group("dataset1"));
    EXPECT_TRUE(node);
    EXPECT_FALSE(dynamic_cast<AttributeGroup*>(node));

    EXPECT_THROW(root.create_attribute("dataset1", Scalar(1)), duplicate_entry);
    EXPECT_THROW(root.create_group("dataset1"), duplicate_entry);
}

TEST_F(oh5_MemoryNodeImpl_test, test_create_group_attributegroup) {
    Group* node = 0;
    EXPECT_NO_THROW(node = &root.create_group("what"));
    EXPECT_TRUE(node);
    EXPECT_TRUE(dynamic_cast<AttributeGroup*>(node));
}

TEST_F(oh5_MemoryNodeImpl_test, test_create_attribute_invalid_names) {
    EXPECT_THROW(root.create_attribute("", Scalar(1)), value_error);
    EXPECT_THROW(root.create_attribute("qwe/asd", Scalar(1)), value_error);
    EXPECT_THROW(root.create_attribute("/", Scalar(1)), value_error);
}

TEST_F(oh5_MemoryNodeImpl_test, test_create_group_invalid_names) {
    EXPECT_THROW(root.create_group(""), value_error);
    EXPECT_THROW(root.create_group("qwe/asd"), value_error);
    EXPECT_THROW(root.create_group("/"), value_error);
}

} // namespace oh5
} // namespace brfc
