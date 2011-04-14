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

#include <brfc/expr/listcons.hpp>
#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace expr {
namespace proc {

namespace {

struct swap {
    typedef sexp result_type;
    
    template<typename T, typename U>
    sexp operator()(const T& a1, const U& a2) {
        sexp result;
        result.push_back(sexp(a1));
        result.push_back(sexp(a2));
        return result;
    }
};

struct unary_cb {
    typedef sexp result_type;

    template<typename T>
    sexp operator()(const T&) {
        return sexp(4321);
    }
};

}

TEST(expr_proc_dispatch_test, test_binary) {
    binary<swap> dp;

    sexp in = listcons().int64(1).int64(2).get();
    sexp out = listcons().int64(2).int64(1).get();

    sexp result;
    EXPECT_NO_THROW(result = dp(in));

    EXPECT_EQ(out, result);
}

TEST(expr_proc_dispatch_test, test_binary_invalid_arg_count) {
    binary<swap> dp;
    
    EXPECT_THROW(dp(sexp()), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_binary_invalid_type) {
    binary<swap> dp;
    
    EXPECT_THROW(dp(sexp(1)), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_unary) {
    unary<unary_cb> dp;
    sexp in = listcons().int64(1).get();
    
    sexp result;
    EXPECT_NO_THROW(result = dp(in));

    EXPECT_EQ(sexp(4321), result);
}

TEST(expr_proc_dispatch_test, test_unary_invalid_arg_count) {
    unary<unary_cb> dp;
    
    EXPECT_THROW(dp(sexp()), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_unary_invalid_type) {
    unary<unary_cb> dp;
    
    EXPECT_THROW(dp(sexp(1)), std::logic_error);
}


} // namespace proc
} // namespace expr
} // namespace brfc
