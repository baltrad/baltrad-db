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

#include <brfc/expr/Listcons.hpp>
#include <brfc/expr/proc/list_filter.hpp>
#include <brfc/expr/proc/comparison.hpp>

namespace brfc {
namespace proc {

TEST(expr_proc_list_filter_test, test_call_list_plain) {
    lt cb;
    list_filter p(cb);

    Expression in_l = Listcons().int64(10).int64(20).get();
    EXPECT_EQ(Expression(true), p(Listcons().append(in_l).int64(11).get()));
    EXPECT_EQ(Expression(false), p(Listcons().append(in_l).int64(10).get()));
}

TEST(expr_proc_list_filter_test, test_call_plain_list) {
    lt cb;
    list_filter p(cb);

    Expression in_r = Listcons().int64(10).int64(20).get();
    EXPECT_EQ(Expression(true), p(Listcons().int64(11).append(in_r).get()));
    EXPECT_EQ(Expression(false), p(Listcons().int64(20).append(in_r).get()));
}

TEST(expr_proc_list_filter_test, test_call_list_list) {
    lt cb;
    list_filter p(cb);

    Expression in_l = Listcons().int64(10).int64(20).get();
    Expression in_r = Listcons().int64(21).int64(30).get();
    
    EXPECT_EQ(Expression(true), p(Listcons().append(in_l).append(in_r).get()));
    EXPECT_EQ(Expression(false), p(Listcons().append(in_r).append(in_l).get()));
}

TEST(expr_proc_list_filter_test, test_call_plain_plain) {
    lt cb;
    list_filter p(cb);
    
    EXPECT_EQ(Expression(true), p(Listcons().int64(10).int64(11).get()));
    EXPECT_EQ(Expression(false), p(Listcons().int64(11).int64(10).get()));
}

} // namespace proc
} // namespace brfc
