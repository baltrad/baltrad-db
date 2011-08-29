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
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5MemoryNodeBackend.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

struct oh5_Oh5Attribute_test : public ::testing::Test {
  public:
    oh5_Oh5Attribute_test()
            : attr("attr", Oh5Scalar(1)) {
    }

    virtual void SetUp() {

    }
    
    Oh5Attribute attr;
};

TEST_F(oh5_Oh5Attribute_test, test_full_name) {
    Oh5MemoryNodeBackend be;

    Oh5Attribute& a1 = static_cast<Oh5Attribute&>(be.root().add(new Oh5Attribute("a1", Oh5Scalar(1))));
    EXPECT_EQ("a1", a1.full_name());

    Oh5Group& what = static_cast<Oh5Group&>(be.root().add(new Oh5Group("what")));
    Oh5Attribute& a2 = static_cast<Oh5Attribute&>(what.add(new Oh5Attribute("a2", Oh5Scalar(1))));
    EXPECT_EQ("what/a2", a2.full_name());
}

TEST_F(oh5_Oh5Attribute_test, test_accepts_child_Attribute) {
    Oh5Attribute node("attr", Oh5Scalar(1));
    EXPECT_FALSE(attr.accepts_child(node));
}

} // namespace brfc
