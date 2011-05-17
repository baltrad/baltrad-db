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

#include <stdexcept>

#include <brfc/test_common.hpp>
#include <brfc/expr/Listcons.hpp>
#include <brfc/expr/proc/arithmetic.hpp>

namespace brfc {
namespace proc {

/* add */

TEST(expr_proc_arithmetic_add_test, test_Expression_dispatch) {
    EXPECT_EQ(Expression(3), add()(Listcons().int64(1).int64(2).get()));
}

TEST(expr_proc_arithmetic_add_test, test_int_int) {
    EXPECT_EQ(Expression(3), add()(1, 2));
}

TEST(expr_proc_arithmetic_add_test, test_int_float) {
    EXPECT_EQ(Expression(3.5), add()(1, 2.5));
}

TEST(expr_proc_arithmetic_add_test, test_int_list) {
    EXPECT_THROW(add()(1, Expression::list_t()), std::logic_error);
}

/* sub */

TEST(expr_proc_arithmetic_sub_test, test_Expression_dispatch) {
    EXPECT_EQ(Expression(-1), sub()(Listcons().int64(1).int64(2).get()));
}

TEST(expr_proc_arithmetic_sub_test, test_int_int) {
    EXPECT_EQ(Expression(-1), sub()(1, 2));
}

TEST(expr_proc_arithmetic_sub_test, test_int_float) {
    EXPECT_EQ(Expression(-1.5), sub()(1, 2.5));
}

TEST(expr_proc_arithmetic_sub_test, test_int_list) {
    EXPECT_THROW(sub()(1, Expression::list_t()), std::logic_error);
}

/* mul */

TEST(expr_proc_arithmetic_mul_test, test_Expression_dispatch) {
    EXPECT_EQ(Expression(6), mul()(Listcons().int64(2).int64(3).get()));
}

TEST(expr_proc_arithmetic_mul_test, test_int_int) {
    EXPECT_EQ(Expression(6), mul()(2, 3));
}

TEST(expr_proc_arithmetic_mul_test, test_int_float) {
    EXPECT_EQ(Expression(5.0), mul()(2, 2.5));
}

TEST(expr_proc_arithmetic_mul_test, test_int_list) {
    EXPECT_THROW(mul()(1, Expression::list_t()), std::logic_error);
}

/* div */

TEST(expr_proc_arithmetic_div_test, test_Expression_dispatch) {
    EXPECT_EQ(Expression(1), div()(Listcons().int64(3).int64(2).get()));
}

TEST(expr_proc_arithmetic_div_test, test_int_int) {
    EXPECT_EQ(Expression(0), div()(2, 3));
    EXPECT_EQ(Expression(1), div()(3, 2));
}

TEST(expr_proc_arithmetic_div_test, test_int_float) {
    EXPECT_EQ(Expression(2), div()(5, 2.5));
    EXPECT_EQ(Expression(3.5), div()(7.0, 2));
}

TEST(expr_proc_arithmetic_div_test, test_int_list) {
    EXPECT_THROW(div()(1, Expression::list_t()), std::logic_error);
}

} // namespace proc
} // namespace brfc
