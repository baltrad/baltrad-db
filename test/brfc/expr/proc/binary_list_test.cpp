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
#include <brfc/expr/proc/binary_list.hpp>
#include <brfc/expr/proc/arithmetic.hpp>

namespace brfc {
namespace expr {
namespace proc {

TEST(expr_proc_binary_list_test, test_call_list_plain) {
    div cb;
    binary_list p(cb);

    Expression in_l = Listcons().int64(10).int64(20).get();
    Expression in_r(10);
    Expression out = Listcons().int64(1).int64(2).get();

    EXPECT_EQ(out, p(Listcons().append(in_l).append(in_r).get()));
}

TEST(expr_proc_unary_list_test, test_call_plain_list) {
    div cb;
    binary_list p(cb);

    Expression in_l(10);
    Expression in_r = Listcons().int64(2).int64(1).get();
    Expression out = Listcons().int64(5).int64(10).get();

    EXPECT_EQ(out, p(Listcons().append(in_l).append(in_r).get()));
}

TEST(expr_proc_unary_list_test, test_call_list_list) {
    div cb;
    binary_list p(cb);

    Expression in_l = Listcons().int64(10).int64(40).get();
    Expression in_r = Listcons().int64(5).int64(5).get();
    Expression out = Listcons().int64(2).int64(8).get();

    EXPECT_EQ(out, p(Listcons().append(in_l).append(in_r).get()));
}

TEST(expr_proc_unary_list_test, test_call_plain_plain) {
    div cb;
    binary_list p(cb);

    Expression in_l(12);
    Expression in_r(6);
    Expression out(2);

    EXPECT_EQ(out, p(Listcons().append(in_l).append(in_r).get()));
}

} // namespace proc
} // namespace expr
} // namespace brfc
