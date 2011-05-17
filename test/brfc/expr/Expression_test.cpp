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

#include <brfc/expr/Expression.hpp>

namespace brfc {

TEST(expr_Expression_test, test_ctor_default) {
    Expression e;
    ASSERT_EQ(Expression::type::LIST, e.type());
}

TEST(expr_Expression_test, test_ctor_symbol) {
    Expression e("symbol", Expression::construct_symbol());
    ASSERT_EQ(Expression::type::SYMBOL, e.type());
    ASSERT_EQ("symbol", e.symbol());
}

TEST(expr_Expression_test, test_ctor_string) {
    Expression e("string");
    ASSERT_EQ(Expression::type::STRING, e.type());
    ASSERT_EQ("string", e.string());
}

TEST(expr_Expression_test, test_ctor_bool) {
    Expression e(true);
    ASSERT_EQ(Expression::type::BOOL, e.type());
    ASSERT_TRUE(e.bool_());
}

TEST(expr_Expression_test, test_swap) {
    Expression e1(true);
    Expression e2("foo");
    e1.swap(e2);
    ASSERT_EQ(Expression::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(Expression::type::BOOL, e2.type());
    EXPECT_TRUE(e2.bool_());
}

TEST(expr_Expression_test, test_assign) {
    Expression e1;
    Expression e2("foo");
    e1 = e2;
    ASSERT_EQ(Expression::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(Expression::type::STRING, e2.type());
    EXPECT_EQ("foo", e2.string());
}

TEST(expr_Expression_test, test_copy_ctor) {
    Expression e1("foo");
    Expression e2(e1);
    ASSERT_EQ(Expression::type::STRING, e1.type());
    EXPECT_EQ("foo", e1.string());
    ASSERT_EQ(Expression::type::STRING, e2.type());
    EXPECT_EQ("foo", e2.string());
}

TEST(expr_Expression_test, test_push_back) {
    Expression e;
    e.push_back(Expression("foo"));
    ASSERT_EQ(1u, e.size());
    ASSERT_EQ(Expression::type::STRING, e.front().type());
    ASSERT_EQ("foo", e.front().string());
}

TEST(expr_Expression_test, test_pop_front) {
    Expression e;
    e.push_back(Expression("foo"));
    e.pop_front();
    EXPECT_TRUE(e.empty());
}

TEST(expr_Expression_test, test_pop_front_empty) {
    Expression e;
    EXPECT_ANY_THROW(e.pop_front());
}

TEST(expr_Expression_test, test_equality) {
    Expression e1;
    Expression e2(1);
    Expression e3(1.0f);
    Expression e4(true);
    Expression e5("foo");
    Expression e6("foo", Expression::construct_symbol());
    Expression e7;
    e7.push_back(Expression("foo"));
    Expression e8(Date(2010, 11, 12));
    Expression e9(Time(13, 14, 15));
    Expression e10(DateTime(2010, 11, 12, 13, 14, 15));

    EXPECT_EQ(e1, e1);
    EXPECT_NE(e1, e2);
    EXPECT_NE(e1, e3);
    EXPECT_NE(e1, e4);
    EXPECT_NE(e1, e5);
    EXPECT_NE(e1, e6);
    EXPECT_NE(e1, e7);
    EXPECT_NE(e1, e8);
    EXPECT_NE(e1, e9);
    EXPECT_NE(e1, e10);

    EXPECT_EQ(e2, e2);
    EXPECT_EQ(e2, e3);
    EXPECT_EQ(e2, e4);
    EXPECT_NE(e2, e5);
    EXPECT_NE(e2, e6);
    EXPECT_NE(e2, e7);
    EXPECT_NE(e2, e8);
    EXPECT_NE(e2, e9);
    EXPECT_NE(e2, e10);

    EXPECT_EQ(e3, e3);
    EXPECT_EQ(e3, e4);
    EXPECT_NE(e3, e5);
    EXPECT_NE(e3, e6);
    EXPECT_NE(e3, e7);
    EXPECT_NE(e3, e8);
    EXPECT_NE(e3, e9);
    EXPECT_NE(e3, e10);

    EXPECT_EQ(e4, e4);
    EXPECT_NE(e4, e5);
    EXPECT_NE(e4, e6);
    EXPECT_NE(e4, e7);
    EXPECT_NE(e4, e8);
    EXPECT_NE(e4, e9);
    EXPECT_NE(e4, e10);

    EXPECT_EQ(e5, e5);
    EXPECT_NE(e5, e6);
    EXPECT_NE(e5, e7);
    EXPECT_NE(e5, e8);
    EXPECT_NE(e5, e9);
    EXPECT_NE(e5, e10);

    EXPECT_EQ(e6, e6);
    EXPECT_NE(e6, e7);
    EXPECT_NE(e6, e8);
    EXPECT_NE(e6, e9);
    EXPECT_NE(e6, e10);

    EXPECT_EQ(e7, e7);
    EXPECT_NE(e7, e8);
    EXPECT_NE(e7, e9);
    EXPECT_NE(e7, e10);

    EXPECT_EQ(e8, e8);
    EXPECT_NE(e8, e9);
    EXPECT_NE(e8, e10);

    EXPECT_EQ(e9, e9);
    EXPECT_NE(e9, e10);

    EXPECT_EQ(e10, e10);
}

TEST(expr_Expression_test, test_less_than_list) {
    Expression e1;
    Expression e2;
    e2.push_back(Expression(1));
    Expression e3;
    e3.push_back(Expression(1));
    e3.push_back(Expression(2));
    Expression e4;
    e4.push_back(Expression(2));

    EXPECT_LT(e1, e2);
    EXPECT_LT(e1, e3);
    EXPECT_LT(e1, e4);

    EXPECT_LT(e2, e3);
    EXPECT_LT(e2, e4);

    EXPECT_LT(e3, e4);
}

TEST(expr_Expression_test, test_to_ostream) {
    Expression e;
    e.push_back(Expression("foo", Expression::construct_symbol()));
    e.push_back(Expression("bar"));
    e.push_back(Expression(false));
    e.push_back(Expression(1));
    e.push_back(Expression(1.2));
    e.push_back(Expression());
    e.push_back(Expression(Date(2011, 12, 13)));
    e.push_back(Expression(Time(14, 15, 16, 17)));
    e.push_back(Expression(DateTime(2011, 12, 13, 14, 15, 16, 17)));
    std::stringstream ss;
    ss << e;
    std::string expected =
        std::string("(foo \"bar\" false 1 1.2 () (date \"20111213\") ") +
        "(time \"141516.017000\") (datetime \"20111213T141516.017000\"))";
    EXPECT_EQ(expected, ss.str());
}

namespace {

struct noncopyable_visitor : static_visitor<void> {
    noncopyable_visitor() { }

    template<typename T>
    void operator()(const T& t) { }
  
  private:
    noncopyable_visitor(const noncopyable_visitor&);
    noncopyable_visitor& operator=(const noncopyable_visitor&);
};

struct noncopyable_const_visitor : static_visitor<void> {
    noncopyable_const_visitor() { }

    template<typename T>
    void operator()(const T& t) const { }
  
  private:
    noncopyable_const_visitor(const noncopyable_const_visitor&);
    noncopyable_const_visitor& operator=(const noncopyable_const_visitor&);
};

}

TEST(expr_Expression_test, test_apply_visitor_noncopyable_visitor) {
    Expression e;
    noncopyable_visitor v;
    apply_visitor(v, e);
}

TEST(expr_Expression_test, test_apply_visitor_noncopyable_const_visitor) {
    Expression e;
    noncopyable_const_visitor v;
    apply_visitor(v, e);
}

TEST(expr_Expression_test, test_bool_conversion) {
    EXPECT_FALSE(Expression());
    EXPECT_TRUE(Expression(1));
    EXPECT_FALSE(Expression(false));
}

TEST(expr_Expression_test, test_extend) {
    Expression e;
    Expression l;
    l.push_back(Expression(1));
    l.push_back(Expression(2));
    e.extend(l);

    EXPECT_EQ(l, e);
}

TEST(expr_Expression_test, test_join) {
    Expression e;
    e.push_back(Expression(1));
    e.push_back(Expression(2));
    e.push_back(Expression(3));

    Expression sep(0);

    Expression out;
    e.push_back(Expression(1));
    e.push_back(Expression(0));
    e.push_back(Expression(2));
    e.push_back(Expression(0));
    e.push_back(Expression(3));
}

TEST(expr_Expression_test, test_contains) {
    Expression e;
    e.push_back(Expression(1));
    e.push_back(Expression("asd"));

    EXPECT_TRUE(e.contains(Expression(1)));
    EXPECT_TRUE(e.contains(Expression("asd")));
    EXPECT_FALSE(e.contains(Expression()));
}

} // namespace brfc
