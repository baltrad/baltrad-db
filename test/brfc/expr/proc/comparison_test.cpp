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
#include <brfc/expr/proc/comparison.hpp>

namespace brfc {
namespace expr {
namespace proc {

/* eq */

TEST(expr_proc_comparison_eq_test, test_int_int) {
    EXPECT_EQ(Expression(true), eq()(Listcons().int64(1).int64(1).get()));
    EXPECT_EQ(Expression(false), eq()(Listcons().int64(1).int64(2).get()));
}

/* ne */

TEST(expr_proc_comparison_ne_test, test_int_int) {
    EXPECT_EQ(Expression(false), ne()(Listcons().int64(1).int64(1).get()));
    EXPECT_EQ(Expression(true), ne()(Listcons().int64(1).int64(2).get()));
}

/* lt */

TEST(expr_proc_comparison_lt_test, test_int_int) {
    EXPECT_EQ(Expression(true), lt()(Listcons().int64(1).int64(2).get()));
    EXPECT_EQ(Expression(false), lt()(Listcons().int64(1).int64(1).get()));
}

/* le */

TEST(expr_proc_comparison_le_test, test_int_int) {
    EXPECT_EQ(Expression(true), le()(Listcons().int64(1).int64(2).get()));
    EXPECT_EQ(Expression(true), le()(Listcons().int64(1).int64(1).get()));
    EXPECT_EQ(Expression(false), le()(Listcons().int64(2).int64(1).get()));
}

/* gt */

TEST(expr_proc_comparison_gt_test, test_int_int) {
    EXPECT_EQ(Expression(true), gt()(Listcons().int64(2).int64(1).get()));
    EXPECT_EQ(Expression(false), gt()(Listcons().int64(1).int64(1).get()));
}

/* ge */

TEST(expr_proc_comparison_ge_test, test_int_int) {
    EXPECT_EQ(Expression(true), ge()(Listcons().int64(2).int64(1).get()));
    EXPECT_EQ(Expression(true), ge()(Listcons().int64(1).int64(1).get()));
    EXPECT_EQ(Expression(false), ge()(Listcons().int64(1).int64(2).get()));
}


} // namespace proc
} // namespace expr
} // namespace brfc
