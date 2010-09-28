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
#include <brfc/oh5/RootGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Attribute_test : public ::testing::Test {
  public:
    oh5_Attribute_test()
            : attr(0, "attr", Scalar(1)) {

    }
    
    Attribute attr;
};

TEST_F(oh5_Attribute_test, test_full_name) {
    RootGroup root;
    Attribute& a1 = root.create_child_attribute("a1", Scalar(1));
    EXPECT_EQ("a1", a1.full_name());

    AttributeGroup what(0, "what");
    Attribute& a2 = what.create_child_attribute("a2", Scalar(1));
    EXPECT_EQ("what/a2", a2.full_name());
}

TEST_F(oh5_Attribute_test, test_add_child_Attribute) {
    auto_ptr<Node> a(new Attribute(0, "a", Scalar(1)));
    EXPECT_THROW(attr.add_child(a), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_AttributeGroup) {
    auto_ptr<Node> what(new AttributeGroup(0, "what"));
    EXPECT_THROW(attr.add_child(what), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_RootGroup) {
    auto_ptr<Node> root(new RootGroup());
    EXPECT_THROW(attr.add_child(root), value_error);
}

} // namepsace oh5
} // namespace brfc
