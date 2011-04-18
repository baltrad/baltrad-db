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

#include <brfc/expr/proc/logic.hpp>

namespace brfc {
namespace expr {
namespace proc {

TEST(expr_proc_logic_and_test, test_bool_bool) {
    EXPECT_EQ(sexp(true), and_()(true, true));
    EXPECT_EQ(sexp(false), and_()(true, false));
}

TEST(expr_proc_logic_and_test, test_bool_int) {
    EXPECT_THROW(and_()(true, 1), std::logic_error);
}

TEST(expr_proc_logic_or_test, test_bool_bool) {
    EXPECT_EQ(sexp(true), or_()(true, false));
    EXPECT_EQ(sexp(false), or_()(false, false));
}

TEST(expr_proc_logic_or_test, test_bool_int) {
    EXPECT_THROW(or_()(true, 1), std::logic_error);
}

TEST(expr_proc_logic_not_test, test_bool) {
    EXPECT_EQ(sexp(true), not_()(false));
    EXPECT_EQ(sexp(false), not_()(true));
}

TEST(expr_proc_logic_not_test, test_int) {
    EXPECT_THROW(not_()(1), std::logic_error);
}

} // namespace proc
} // namespace expr
} // namespace brfc