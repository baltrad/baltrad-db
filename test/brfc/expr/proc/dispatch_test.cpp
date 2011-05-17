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

#include <stdexcept>

#include <boost/ref.hpp>

#include <gtest/gtest.h>

#include <brfc/test_common.hpp>
#include <brfc/expr/Listcons.hpp>
#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace proc {

namespace {

struct binary_op {
    typedef void result_type;

    mutable Expression arg1, arg2;
    
    template<typename T, typename U>
    result_type operator()(const T& a1, const U& a2) const {
        arg1 = Expression(a1);
        arg2 = Expression(a2);
    }
};

struct unary_op {
    typedef void result_type;

    mutable Expression arg;

    template<typename T>
    void operator()(const T& a) const {
        arg = Expression(a);
    }
};

}

TEST(expr_proc_dispatch_test, test_binary) {
    binary_op op;
    Expression in = Listcons().int64(1).int64(2).get();

    EXPECT_NO_THROW(binary_dispatch(op, in));
    
    EXPECT_EQ(Expression(1), op.arg1);
    EXPECT_EQ(Expression(2), op.arg2);
}

TEST(expr_proc_dispatch_test, test_binary_invalid_arg_count) {
    binary_op op;
    
    EXPECT_THROW(binary_dispatch(op, Expression()), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_binary_invalid_type) {
    binary_op op;
    
    EXPECT_THROW(binary_dispatch(op, Expression(1)), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_unary) {
    unary_op op;
    Expression in = Listcons().int64(1).get();
    
    EXPECT_NO_THROW(unary_dispatch(op, in));

    EXPECT_EQ(Expression(1), op.arg);
}

TEST(expr_proc_dispatch_test, test_unary_invalid_arg_count) {
    unary_op op;
    
    EXPECT_THROW(unary_dispatch(op, Expression()), std::logic_error);
}

TEST(expr_proc_dispatch_test, test_unary_invalid_type) {
    unary_op op;
    
    EXPECT_THROW(unary_dispatch(op, Expression(1)), std::logic_error);
}


} // namespace proc
} // namespace brfc
