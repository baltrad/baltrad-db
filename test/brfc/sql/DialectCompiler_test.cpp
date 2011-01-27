/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Bind.hpp>
#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/DialectCompiler.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Function.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockDialect.hpp>

using ::testing::_;
using ::testing::Invoke;

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
            , xpr()
            , t1(Table::create("t1"))
            , t2(Table::create("t2"))
            , t3(Table::create("t3")) {
    }

    virtual void SetUp() {
        ON_CALL(dialect, do_variant_to_string(_))
            .WillByDefault(Invoke(&replacer, &Replacer::replace));

        t1->add_column("c1");
        t1->add_column("c2");
        t1->add_column("c3");
        t2->add_column("c1");
        t2->add_column("c2");
        t2->add_column("c3");
        t3->add_column("c1");
        t3->add_column("c2");
        t3->add_column("c3");
    }
    
    Replacer replacer;
    ::testing::NiceMock<MockDialect> dialect;
    DialectCompiler compiler;
    Factory xpr;
    TablePtr t1, t2, t3;
};

TEST_F(sql_DialectCompiler_test, test_simple) {
    ExpressionPtr expr = xpr.int64_(1)->lt(xpr.int64_(2));
    const Query& q = compiler.compile(*expr);
    EXPECT_EQ(":lit_0 < :lit_1", q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_1"));
}

TEST_F(sql_DialectCompiler_test, test_between) {
    ExpressionPtr expr = xpr.int64_(1)->between(xpr.int64_(0), xpr.int64_(2));
    const Query& q = compiler.compile(*expr);
    EXPECT_EQ(":lit_0 >= :lit_1 AND :lit_2 <= :lit_3", q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(0), replacer.value(":lit_1"));
    EXPECT_EQ(Variant(1), replacer.value(":lit_2"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_3"));
}

TEST_F(sql_DialectCompiler_test, test_string_literal) {
    LiteralPtr l = xpr.string("a string");
    const Query& q = compiler.compile(*l);
    EXPECT_EQ(":lit_0", q.statement());
    EXPECT_EQ(Variant("a string"), replacer.value(":lit_0"));
}

TEST_F(sql_DialectCompiler_test, test_parentheses) {
    ExpressionPtr expr = xpr.int64_(1)->parentheses();
    const Query& q = compiler.compile(*expr);
    EXPECT_EQ("(:lit_0)", q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
}

TEST_F(sql_DialectCompiler_test, test_column) {
    ColumnPtr c = t1->column("c1");
    const Query& q = compiler.compile(*c);
    EXPECT_EQ("t1.c1", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_bind) {
    BindPtr b = Bind::create(":bind");
    const Query& q = compiler.compile(*b);
    EXPECT_EQ(":bind", q.statement());
    ASSERT_TRUE(q.binds().has("bind"));
    EXPECT_EQ(Variant(), q.binds().get("bind"));
}

TEST_F(sql_DialectCompiler_test, test_bind_without_colon) {
    BindPtr b = Bind::create("bind");
    const Query& q = compiler.compile(*b);
    EXPECT_EQ(":bind", q.statement());
    ASSERT_TRUE(q.binds().has("bind"));
    EXPECT_EQ(Variant(), q.binds().get("bind"));
}

TEST_F(sql_DialectCompiler_test, test_aliased_table_column) {
    AliasPtr a = t1->alias("table_alias");
    ColumnPtr c = a->column("c1");
    const Query& q = compiler.compile(*c);
    EXPECT_EQ("table_alias.c1", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_aliased_select) {
    SelectPtr subq = Select::create();
    subq->from(t1);
    subq->what(t1->column("c1"));
    AliasPtr a = subq->alias("a");
    SelectPtr select = Select::create();
    select->from(a);
    select->what(a->column("c1"));
    std::string expected("SELECT a.c1 FROM (SELECT t1.c1 FROM t1) AS a");
    const Query& q = compiler.compile(*select);
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_basic_join) {
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("c2")));
    const Query& q = compiler.compile(*j);
    EXPECT_EQ("t1 JOIN t2 ON t1.c1 = t2.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_outerjoin) {
    JoinPtr j = t1->outerjoin(t2, t1->column("c1")->eq(t2->column("c2")));
    const Query& q = compiler.compile(*j);
    EXPECT_EQ("t1 LEFT JOIN t2 ON t1.c1 = t2.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_join_with_alias) {
    SelectablePtr a = t2->alias("a");
    JoinPtr j = t1->join(a, t1->column("c1")->eq(a->column("c2")));
    const Query& q = compiler.compile(*j);
    EXPECT_EQ("t1 JOIN t2 AS a ON t1.c1 = a.c2", q.statement());
}

TEST_F(sql_DialectCompiler_test, test_join_with_multiple_aliases) {
    SelectablePtr a1 = t1->alias("a1");
    SelectablePtr a2 = t2->alias("a2");
    SelectablePtr a3 = t3->alias("a3");

    JoinPtr j = a1->join(a2, a1->column("c1")->eq(a2->column("c2")));
    j = j->join(a3, a2->column("c2")->eq(a3->column("c3")));
    const Query& q = compiler.compile(*j);

    EXPECT_EQ("t1 AS a1 JOIN t2 AS a2 ON a1.c1 = a2.c2 JOIN t3 AS a3 ON a2.c2 = a3.c3",
              q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select) {
    SelectPtr select = Select::create();
    ColumnPtr column = t1->column("c1");
    ColumnPtr column2 = t1->column("c2");
    ColumnPtr column3 = t2->column("c3");
    select->from(t1->crossjoin(t2));
    select->what(column);
    select->what(column2);
    select->what(column3);
    select->where(column->lt(xpr.int64_(1)));
    std::string expected("SELECT t1.c1, t1.c2, t2.c3 FROM t1 CROSS JOIN t2 WHERE t1.c1 < :lit_0");
    const Query& q = compiler.compile(*select);
    EXPECT_EQ(expected, q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
}

TEST_F(sql_DialectCompiler_test, test_select_group_by) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->append_group_by(t1->column("c1"));
    select->append_group_by(t1->column("c2"));
    const Query& q = compiler.compile(*select);
    std::string expected = "SELECT t1.c1 FROM t1 GROUP BY t1.c1, t1.c2";
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_order_by) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->what(t1->column("c2"));
    select->append_order_by(t1->column("c1"), Select::ASC);
    select->append_order_by(t1->column("c3"), Select::DESC);
    const Query& q = compiler.compile(*select);
    std::string expected = "SELECT t1.c1, t1.c2 FROM t1 ORDER BY t1.c1 ASC, t1.c3 DESC";
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_order_by_asc) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->append_order_by(t1->column("c1"), Select::ASC);
    const Query& q = compiler.compile(*select);
    std::string expected("SELECT t1.c1 FROM t1 ORDER BY t1.c1 ASC");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_order_by_desc) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->append_order_by(t1->column("c1"), Select::DESC);
    const Query& q = compiler.compile(*select);
    std::string expected("SELECT t1.c1 FROM t1 ORDER BY t1.c1 DESC");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_limit) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->limit(1);
    const Query& q = compiler.compile(*select);
    std::string expected("SELECT t1.c1 FROM t1 LIMIT 1");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_offset) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->offset(2);
    const Query& q = compiler.compile(*select);
    std::string expected("SELECT t1.c1 FROM t1 OFFSET 2");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_select_offset_with_limit) {
    SelectPtr select = Select::create();
    select->from(t1);
    select->what(t1->column("c1"));
    select->limit(1);
    select->offset(2);
    const Query& q = compiler.compile(*select);
    std::string expected("SELECT t1.c1 FROM t1 LIMIT 1 OFFSET 2");
    EXPECT_EQ(expected, q.statement());
}

TEST_F(sql_DialectCompiler_test, test_insert) {
    InsertPtr insert = Insert::create(t1);
    insert->value("c1", xpr.int64_(1));
    insert->value("c2", xpr.int64_(2));
    insert->add_return(t1->column("c3"));
    std::string expected("INSERT INTO t1(c1, c2) VALUES (:lit_0, :lit_1) RETURNING t1.c3");
    const Query& q = compiler.compile(*insert);
    EXPECT_EQ(expected, q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_1"));
}

TEST_F(sql_DialectCompiler_test, test_factory_or_) {
    ExpressionPtr e1 = t1->column("c1")->eq(xpr.int64_(1));
    ExpressionPtr e2 = t1->column("c1")->eq(xpr.int64_(2));
    ExpressionPtr e3 = xpr.or_(e1, e2);
    std::string expected("t1.c1 = :lit_0 OR t1.c1 = :lit_1");
    const Query& q = compiler.compile(*e3);
    EXPECT_EQ(expected, q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_1"));
}

TEST_F(sql_DialectCompiler_test, test_function) {
    FunctionPtr f = Function::create("func1");
    f->add_arg(xpr.int64_(1));
    f->add_arg(xpr.int64_(2));
    const Query& q = compiler.compile(*f);
    EXPECT_EQ("func1(:lit_0, :lit_1)", q.statement());
    EXPECT_EQ(Variant(1), replacer.value(":lit_0"));
    EXPECT_EQ(Variant(2), replacer.value(":lit_1"));
}

} // namespace sql
} // namespace brfc

