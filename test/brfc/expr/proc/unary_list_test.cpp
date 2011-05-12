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
#include <brfc/expr/proc/logic.hpp>
#include <brfc/expr/proc/unary_list.hpp>

namespace brfc {
namespace expr {
namespace proc {

TEST(expr_proc_unary_list_test, test_call_list) {
    not_ cb;
    unary_list p(cb);

    Expression in = Listcons().append(Listcons().bool_(true).bool_(false).get()).get();
    Expression out = Listcons().bool_(false).bool_(true).get();

    EXPECT_EQ(out, p(in));
}

TEST(expr_proc_unary_list_test, test_call_plain) {
    not_ cb;
    unary_list p(cb);

    Expression in = Listcons().bool_(false).get();
    Expression out(true);

    EXPECT_EQ(out, p(in));
}

} // namespace proc
} // namespace expr
} // namespace brfc
