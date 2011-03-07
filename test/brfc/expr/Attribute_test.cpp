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

#include <brfc/expr/Attribute.hpp>

namespace brfc {
namespace expr {

TEST(expr_Attribute_test, test_equals) {
    Attribute a1("a1", Attribute::STRING);
    Attribute a2("a1", Attribute::INT64);
    Attribute a3("a2", Attribute::STRING);
    
    EXPECT_TRUE(a1.equals(a1));
    EXPECT_FALSE(a1.equals(a2));
    EXPECT_FALSE(a1.equals(a3));
    EXPECT_FALSE(a2.equals(a1));
    EXPECT_TRUE(a2.equals(a2));
    EXPECT_FALSE(a2.equals(a3));
    EXPECT_FALSE(a3.equals(a1));
    EXPECT_FALSE(a3.equals(a2));
    EXPECT_TRUE(a3.equals(a3));
}

} // namespace expr
} // namespace brfc
