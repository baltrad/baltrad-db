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
#include <string>

#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include <brfc/Variant.hpp>

#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/DialectCompiler.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockDialect.hpp>

using ::testing::_;
using ::testing::Invoke;
using ::brfc::expr::Expression;

namespace brfc {
namespace sql {

namespace {

class Replacer {
  public:
    Replacer()
            : binds_()
            , count_(0) {
    }

    std::string replace(const Variant& value) {
        std::string key = ":lit_" + boost::lexical_cast<std::string>(count_++);
        binds_.add(key, value);
        return key;
    }

    Variant value(const std::string& key) const {
        return binds_.get(key, Variant());
    }

  private:
    BindMap binds_;
    int count_;
};

} // namespace anonymous

struct sql_DialectCompiler_test: public testing::Test {
    sql_DialectCompiler_test()
            : dialect()
            , compiler(&dialect)
            , xpr() {
    }

    virtual void SetUp() {
        ON_CALL(dialect, do_variant_to_string(_))
            .WillByDefault(Invoke(&replacer, &Replacer::replace));
    }
    
    Replacer replacer;
    ::testing::NiceMock<MockDialect> dialect;
    DialectCompiler compiler;
    Factory xpr;
};

TEST_F(sql_DialectCompiler_test, test_simple) {
    Expression expr = xpr.lt(xpr.int64_(1), xpr.int64_(2));
    const Query& q = compiler.compile(expr);
    EXPECT_EQ("1 < 2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_not) {
    Expression expr = xpr.not_(xpr.bool_(true));
    const Query& q = compiler.compile(expr);
    EXPECT_EQ("NOT true", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_between) {
    Expression expr = xpr.between(xpr.int64_(1), xpr.int64_(0), xpr.int64_(2));
    const Query& q = compiler.compile(expr);
    EXPECT_EQ("0 <= 1 AND 1 <= 2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_string_literal) {
    Expression l = xpr.string("a string");
    const Query& q = compiler.compile(l);
    EXPECT_EQ("'a string'", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_column) {
    Expression c = xpr.column("t1", "c1");
    const Query& q = compiler.compile(c);
    EXPECT_EQ("t1.c1", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_bind) {
    Expression b = xpr.bind(":bind");
    const Query& q = compiler.compile(b);
    EXPECT_EQ(":bind", q.statement());
    ASSERT_TRUE(q.binds().has("bind"));
    EXPECT_EQ(Variant(), q.binds().get("bind"));
}

TEST_F(sql_DialectCompiler_test, test_bind_without_colon) {
    Expression b = xpr.bind("bind");
    const Query& q = compiler.compile(b);
    EXPECT_EQ(":bind", q.statement());
    ASSERT_TRUE(q.binds().has("bind"));
    EXPECT_EQ(Variant(), q.binds().get("bind"));
}

TEST_F(sql_DialectCompiler_test, test_aliased_select) {
    Select subq;
    subq.from(xpr.table("t1"));
    subq.what(xpr.column("t1", "c1"));
    Expression a = xpr.alias(subq.expression(), "a");
    Select select;
    select.from(a);
    select.what(xpr.column("a", "c1"));
    std::string expected("SELECT a.c1 FROM (SELECT t1.c1 FROM t1) AS a");
    const Query& q = compiler.compile(select.expression());
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_basic_join) {
    FromClause f(xpr.table("t1"));
    f.join(xpr.table("t2"), xpr.eq(xpr.column("t1", "c1"), xpr.column("t2", "c2")));
    const Query& q = compiler.compile(f.expression());
    EXPECT_EQ("FROM t1 JOIN t2 ON t1.c1 = t2.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_outerjoin) {
    FromClause f(xpr.table("t1"));
    f.outerjoin(xpr.table("t2"), xpr.eq(xpr.column("t1", "c1"), xpr.column("t2", "c2")));
    const Query& q = compiler.compile(f.expression());
    EXPECT_EQ("FROM t1 LEFT JOIN t2 ON t1.c1 = t2.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_join_with_alias) {
    FromClause f(xpr.table("t1"));
    f.join(
        xpr.alias(xpr.table("t2"), "a"),
        xpr.eq(xpr.column("t1", "c1"), xpr.column("a", "c2"))
    );
    const Query& q = compiler.compile(f.expression());
    EXPECT_EQ("FROM t1 JOIN t2 AS a ON t1.c1 = a.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select) {
    Select select;
    select.from(xpr.table("t1"));
    select.join(
        xpr.table("t2"),
        xpr.eq(xpr.column("t1", "c1"), xpr.column("t2", "c2"))
    );
    Expression col = xpr.column("t1", "c1");
    select.what(col);
    select.what(xpr.column("t1", "c2"));
    select.what(xpr.column("t2", "c3"));
    select.where(xpr.lt(col, xpr.int64_(1)));
    std::string expected("SELECT t1.c1, t1.c2, t2.c3 FROM t1 JOIN t2 ON t1.c1 = t2.c2 WHERE t1.c1 < 1");
    const Query& q = compiler.compile(select.expression());
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_group_by) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.append_group_by(xpr.column("t1", "c1"));
    select.append_group_by(xpr.column("t1", "c2"));
    const Query& q = compiler.compile(select.expression());
    std::string expected = "SELECT t1.c1 FROM t1 GROUP BY t1.c1, t1.c2";
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_order_by) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.what(xpr.column("t1", "c2"));
    select.append_order_by(xpr.column("t1", "c1"), Select::ASC);
    select.append_order_by(xpr.column("t1", "c3"), Select::DESC);
    const Query& q = compiler.compile(select.expression());
    std::string expected = "SELECT t1.c1, t1.c2 FROM t1 ORDER BY t1.c1 ASC, t1.c3 DESC";
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_limit) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.limit(1);
    const Query& q = compiler.compile(select.expression());
    std::string expected("SELECT t1.c1 FROM t1 LIMIT 1");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_offset) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.offset(2);
    const Query& q = compiler.compile(select.expression());
    std::string expected("SELECT t1.c1 FROM t1 OFFSET 2");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_offset_with_limit) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.limit(1);
    select.offset(2);
    const Query& q = compiler.compile(select.expression());
    std::string expected("SELECT t1.c1 FROM t1 LIMIT 1 OFFSET 2");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_insert) {
    Insert insert("t1");
    insert.value("c1", xpr.int64_(1));
    insert.value("c2", xpr.int64_(2));
    insert.returning(xpr.column("t1", "c3"));
    std::string expected("INSERT INTO t1 (c1, c2) VALUES (1, 2) RETURNING t1.c3");
    const Query& q = compiler.compile(insert.expression());
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_factory_or_) {
    Expression e1 = xpr.eq(xpr.column("t1", "c1"), xpr.int64_(1));
    Expression e2 = xpr.eq(xpr.column("t1", "c1"), xpr.int64_(2));
    Expression e3 = xpr.or_(e1, e2);
    std::string expected("t1.c1 = 1 OR t1.c1 = 2");
    const Query& q = compiler.compile(e3);
    EXPECT_EQ(expected, q.statement());
}

/*
TEST_F(sql_DialectCompiler_test, test_function) {
    FunctionPtr f = Function::create("func1");
    f->add_arg(xpr.int64_(1));
    f->add_arg(xpr.int64_(2));
    const Query& q = compiler.compile(*f);
    EXPECT_EQ("func1(:lit_0, :lit_1)", q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_1"));
}
*/

} // namespace sql
} // namespace brfc

