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

#include <brfc/expr/Literal.hpp>

namespace brfc {
namespace expr {

TEST(expr_Literal_test, test_equals) {
    Literal l1(Variant(1));
    Literal l2(Variant(2.f));
    
    EXPECT_TRUE(l1.equals(l1));
    EXPECT_FALSE(l1.equals(l2));
    EXPECT_FALSE(l2.equals(l1));
    EXPECT_TRUE(l2.equals(l2));
}

} // namespace expr
} // namespace brfc
