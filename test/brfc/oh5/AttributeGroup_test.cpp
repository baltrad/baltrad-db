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
#include "MockNodeImpl.hpp"

using ::testing::ReturnRef;
using ::testing::Return;

namespace brfc {
namespace oh5 {

struct oh5_AttributeGroup_test : public ::testing::Test {
    oh5_AttributeGroup_test()
            : what(MockNodeImpl::create())
            , impl(static_cast<MockNodeImpl&>(what.impl()))
            , name("what") {
    }

    virtual void SetUp() {
        ON_CALL(impl, do_name())
            .WillByDefault(ReturnRef(name));
        ON_CALL(impl, do_parent())
            .WillByDefault(Return((Node*)0));
    }

    AttributeGroup what;
    MockNodeImpl& impl;
    String name;
};

TEST_F(oh5_AttributeGroup_test, test_accepts_child_Attribute) {
    Attribute node(MockNodeImpl::create(), Scalar(1));
    EXPECT_TRUE(what.accepts_child(node));
}

TEST_F(oh5_AttributeGroup_test, test_accepts_child_AttributeGroup) {
    AttributeGroup node(MockNodeImpl::create());
    EXPECT_FALSE(what.accepts_child(node));
}

TEST_F(oh5_AttributeGroup_test, test_accepts_child_RootGroup) {
    RootGroup node(MockNodeImpl::create());
    EXPECT_FALSE(what.accepts_child(node));
}

} // namespace oh5
} // namespace brfc
