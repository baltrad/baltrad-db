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

struct oh5_AttributeGroup_test : public ::testing::Test {
    oh5_AttributeGroup_test()
            : what(0, "what") {
    }

    AttributeGroup what;
};

TEST_F(oh5_AttributeGroup_test, test_add_child_Attribute) {
    auto_ptr<Node> a(new Attribute(0, "a", Scalar(1)));
    EXPECT_NO_THROW(what.add_child(a));
}

TEST_F(oh5_AttributeGroup_test, test_add_child_AttributeGroup) {
    auto_ptr<Node> where(new AttributeGroup(0, "where"));
    EXPECT_THROW(what.add_child(where), value_error);
}

TEST_F(oh5_AttributeGroup_test, test_add_child_RootGroup) {
    auto_ptr<Node> root(new RootGroup());
    EXPECT_THROW(what.add_child(root), value_error);
}

} // namespace oh5
} // namespace brfc
