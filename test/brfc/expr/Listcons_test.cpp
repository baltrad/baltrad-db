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

namespace brfc {
namespace expr {

TEST(expr_Listcons_test, test_empty) {
    Expression e = Listcons().get();
    ASSERT_TRUE(e.is_list());
    ASSERT_EQ(0u, e.size());
}

TEST(expr_Listcons_test, test_construct_one) {
    Expression e= Listcons().symbol("+").get();
    ASSERT_EQ(1u, e.size());
    ASSERT_EQ(Expression::type::SYMBOL, e.front().type());
}

TEST(expr_Listcons_test, test_construct_many) {
    Expression e = Listcons().symbol("+")
                       .int64(1)
                       .double_(2.1)
                       .bool_(true)
                       .string("foo")
                       .date(2011, 12, 13)
                       .time(14, 15, 16, 17)
                       .datetime(2011, 12, 13, 14, 15, 16, 17)
                       .get();
    ASSERT_EQ(8u, e.size());
    Expression::const_iterator it = e.begin();
    ASSERT_TRUE(it->is_symbol());
    ASSERT_EQ("+", it->symbol());
    ++it;
    ASSERT_TRUE(it->is_int64());
    ASSERT_EQ(1, it->int64());
    ++it;
    ASSERT_TRUE(it->is_double());
    ASSERT_EQ(2.1, it->double_());
    ++it;
    ASSERT_TRUE(it->is_bool());
    ASSERT_TRUE(it->bool_());
    ++it;
    ASSERT_TRUE(it->is_string());
    ASSERT_EQ("foo", it->string());
    ++it;
    ASSERT_TRUE(it->is_date());
    ASSERT_EQ(Date(2011, 12, 13), it->date());
    ++it;
    ASSERT_TRUE(it->is_time());
    ASSERT_EQ(Time(14, 15, 16, 17), it->time());
    ++it;
    ASSERT_TRUE(it->is_datetime());
    ASSERT_EQ(DateTime(2011, 12, 13, 14, 15, 16, 17), it->datetime());
}

}
}
