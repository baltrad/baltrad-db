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
#include <brfc/expr/listcons.hpp>
#include <brfc/expr/proc/in.hpp>

namespace brfc {
namespace expr {
namespace proc {

TEST(expr_proc_in_test, test_float_in_ints) {
    sexp coll = listcons().int64(1).int64(2).int64(3).get();
    sexp args = listcons().double_(1).append(coll).get();
    EXPECT_EQ(sexp(true), in()(args));
    args = listcons().double_(4).append(coll).get();
    EXPECT_EQ(sexp(false), in()(args));
}

TEST(expr_proc_in_test, test_str_in_strs) {
    sexp coll = listcons().string("foo").string("bar").get();
    sexp args = listcons().string("foo").append(coll).get();
    EXPECT_EQ(sexp(true), in()(args));
    args = listcons().string("baz").append(coll).get();
    EXPECT_EQ(sexp(false), in()(args));
}

TEST(expr_proc_in_test, test_invalid_arg_types) {
    sexp args = listcons().append(sexp()).append(sexp()).get();
    EXPECT_THROW(in()(args), std::logic_error);
}

TEST(expr_proc_in_test, test_invalid_arg_count) {
    sexp args = listcons().int64(1).append(sexp()).append(sexp()).get();
    EXPECT_THROW(in()(args), std::logic_error);
}

} // namespace proc
} // namespace expr
} // namespace brfc
