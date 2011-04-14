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

#include <sstream>
#include <stdexcept>

#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {

TEST(expr_sexp_test, test_ctor_default) {
    sexp e;
    ASSERT_EQ(sexp::type::LIST, e.type());
}

TEST(expr_sexp_test, test_ctor_symbol) {
    sexp e("symbol", sexp::construct_symbol());
    ASSERT_EQ(sexp::type::SYMBOL, e.type());
    ASSERT_EQ("symbol", e.symbol());
}

TEST(expr_sexp_test, test_ctor_string) {
    sexp e("string");
    ASSERT_EQ(sexp::type::STRING, e.type());
    ASSERT_EQ("string", e.string());
}

TEST(expr_sexp_test, test_ctor_bool) {
    sexp e(true);
    ASSERT_EQ(sexp::type::BOOL, e.type());
    ASSERT_TRUE(e.bool_());
}

TEST(expr_sexp_test, test_swap) {
    sexp e1(true);
    sexp e2("foo");
    e1.swap(e2);
    ASSERT_EQ(sexp::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(sexp::type::BOOL, e2.type());
    EXPECT_TRUE(e2.bool_());
}

TEST(expr_sexp_test, test_assign) {
    sexp e1;
    sexp e2("foo");
    e1 = e2;
    ASSERT_EQ(sexp::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(sexp::type::STRING, e2.type());
    EXPECT_EQ("foo", e2.string());
}

TEST(expr_sexp_test, test_copy_ctor) {
    sexp e1("foo");
    sexp e2(e1);
    ASSERT_EQ(sexp::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(sexp::type::STRING, e2.type());
    EXPECT_EQ("foo", e2.string());
}

TEST(expr_sexp_test, test_push_back) {
    sexp e;
    e.push_back(sexp("foo"));
    ASSERT_EQ(1u, e.size());
    ASSERT_EQ(sexp::type::STRING, e.front().type());
    ASSERT_EQ("foo", e.front().string());
}

TEST(expr_sexp_test, test_pop_front) {
    sexp e;
    e.push_back(sexp("foo"));
    e.pop_front();
    EXPECT_TRUE(e.empty());
}

TEST(expr_sexp_test, test_pop_front_empty) {
    sexp e;
    EXPECT_ANY_THROW(e.pop_front());
}

TEST(expr_sexp_test, test_equality) {
    sexp e1;
    sexp e2(1);
    sexp e3(1.0f);
    sexp e4(true);
    sexp e5("foo");
    sexp e6("foo", sexp::construct_symbol());
    sexp e7;
    e7.push_back(sexp("foo"));

    EXPECT_EQ(e1, e1);
    EXPECT_NE(e1, e2);
    EXPECT_NE(e1, e3);
    EXPECT_NE(e1, e4);
    EXPECT_NE(e1, e5);
    EXPECT_NE(e1, e6);
    EXPECT_NE(e1, e7);

    EXPECT_EQ(e2, e2);
    EXPECT_EQ(e2, e3);
    EXPECT_EQ(e2, e4);
    EXPECT_NE(e2, e5);
    EXPECT_NE(e2, e6);
    EXPECT_NE(e2, e7);

    EXPECT_EQ(e3, e3);
    EXPECT_EQ(e3, e4);
    EXPECT_NE(e3, e5);
    EXPECT_NE(e3, e6);
    EXPECT_NE(e3, e7);

    EXPECT_EQ(e4, e4);
    EXPECT_NE(e4, e5);
    EXPECT_NE(e4, e6);
    EXPECT_NE(e4, e7);

    EXPECT_EQ(e5, e5);
    EXPECT_NE(e5, e6);
    EXPECT_NE(e5, e7);

    EXPECT_EQ(e6, e6);
    EXPECT_NE(e6, e7);

    EXPECT_EQ(e7, e7);
}

TEST(expr_sexp_test, test_less_than_list) {
    sexp e1;
    sexp e2;
    e2.push_back(sexp(1));
    sexp e3;
    e3.push_back(sexp(1));
    e3.push_back(sexp(2));
    sexp e4;
    e4.push_back(sexp(2));

    EXPECT_LT(e1, e2);
    EXPECT_LT(e1, e3);
    EXPECT_LT(e1, e4);

    EXPECT_LT(e2, e3);
    EXPECT_LT(e2, e4);

    EXPECT_LT(e3, e4);
}

TEST(expr_sexp_test, test_to_ostream) {
    sexp e;
    e.push_back(sexp("foo", sexp::construct_symbol()));
    e.push_back(sexp("bar"));
    e.push_back(sexp(false));
    e.push_back(sexp(1));
    e.push_back(sexp(1.2));
    e.push_back(sexp());
    std::stringstream ss;
    ss << e;
    EXPECT_EQ("(foo \"bar\" false 1 1.2 ())", ss.str());
}

} // namespace expr
} // namespace brfc
