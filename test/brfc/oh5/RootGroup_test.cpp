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
#include "MockFile.hpp"
#include "MockNodeImpl.hpp"

namespace brfc {
namespace oh5 {

struct oh5_RootGroup_test : public ::testing::Test {
    oh5_RootGroup_test()
            : root(auto_ptr<NodeImpl>(new MockNodeImpl())) {
    }
    
    RootGroup root;
};

TEST_F(oh5_RootGroup_test, test_accepts_child_Attribute) {
    Attribute node(MockNodeImpl::create(), Scalar(1));
    EXPECT_TRUE(root.accepts_child(node));
}

TEST_F(oh5_RootGroup_test, test_add_child_AttributeGroup) {
    AttributeGroup node(MockNodeImpl::create());
    EXPECT_TRUE(root.accepts_child(node));
}

TEST_F(oh5_RootGroup_test, test_add_child_RootGroup) {
    RootGroup node(MockNodeImpl::create());
    EXPECT_FALSE(root.accepts_child(node));
}

TEST_F(oh5_RootGroup_test, test_file) {
    EXPECT_FALSE(root.file());
    MockFile f;
    root.file(&f);
    EXPECT_EQ(&f, root.file());
}

/*
// tests functionality implemented at Node level
TEST_F(oh5_RootGroup_test, test_file_through_child_node) {
    Group& what = root.create_group("what");
    EXPECT_FALSE(what.file());
    MockFile f;
    root.file(&f);
    EXPECT_EQ(&f, what.file());
}
*/

} // namespace oh5
} // namespace brfc
