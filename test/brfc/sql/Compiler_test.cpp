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

#include <gtest/gtest.h>

#include <brfc/String.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/FromClause.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

#include "../common.hpp"

namespace brfc {
namespace sql {

struct sql_Compiler_test: public testing::Test {
    sql_Compiler_test()
            : compiler()
            , xpr()
            , t1(Table::create("t1"))
            , t2(Table::create("t2"))
            , t3(Table::create("t3")) {
        t1->add_column(Column::create("c1"));
        t1->add_column(Column::create("c2"));
        t1->add_column(Column::create("c3"));
        t2->add_column(Column::create("c1"));
        t2->add_column(Column::create("c2"));
        t2->add_column(Column::create("c3"));
        t3->add_column(Column::create("c1"));
        t3->add_column(Column::create("c2"));
        t3->add_column(Column::create("c3"));
    }

    const Variant& bind(const String& key) const {
        return compiler.binds().get(key, Variant());
    }

    Compiler compiler;
    Factory xpr;
    TablePtr t1, t2, t3;
};

TEST_F(sql_Compiler_test, test_simple) {
    ExpressionPtr expr = xpr.int64_(1)->lt(xpr.int64_(2));
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), ":lit_0 < :lit_1");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(2));
}

TEST_F(sql_Compiler_test, test_between) {
    ExpressionPtr expr = xpr.int64_(1)->between(xpr.int64_(0), xpr.int64_(2));
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), ":lit_0 >= :lit_1 AND :lit_2 <= :lit_3");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(0));
    EXPECT_EQ(bind(":lit_2"), Variant(1));
    EXPECT_EQ(bind(":lit_3"), Variant(2));
}

TEST_F(sql_Compiler_test, test_string_literal) {
    LiteralPtr l = xpr.string("a string");
    compiler.compile(*l);
    EXPECT_EQ(compiler.compiled(), ":lit_0");
    EXPECT_EQ(bind(":lit_0"), Variant("a string"));
}

TEST_F(sql_Compiler_test, test_parentheses) {
    ExpressionPtr expr = xpr.int64_(1)->parentheses();
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), "(:lit_0)");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
}

TEST_F(sql_Compiler_test, test_column) {
    ColumnPtr c = t1->column("c1");
    compiler.compile(*c);
    EXPECT_EQ(compiler.compiled(), "t1.c1");
}

TEST_F(sql_Compiler_test, test_aliased_table_column) {
    AliasPtr a = t1->alias("table_alias");
    ColumnPtr c = a->column("c1");
    compiler.compile(*c);
    EXPECT_EQ(compiler.compiled(), "table_alias.c1");
}

TEST_F(sql_Compiler_test, test_basic_join) {
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 JOIN t2 ON t1.c1 = t2.c2");
}

TEST_F(sql_Compiler_test, test_outerjoin) {
    JoinPtr j = t1->outerjoin(t2, t1->column("c1")->eq(t2->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 LEFT JOIN t2 ON t1.c1 = t2.c2");
}



TEST_F(sql_Compiler_test, test_join_with_alias) {
    SelectablePtr a = t2->alias("a");
    JoinPtr j = t1->join(a, t1->column("c1")->eq(a->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 JOIN t2 AS a ON t1.c1 = a.c2");
}

TEST_F(sql_Compiler_test, test_join_with_multiple_aliases) {
    SelectablePtr a1 = t1->alias("a1");
    SelectablePtr a2 = t2->alias("a2");
    SelectablePtr a3 = t3->alias("a3");

    JoinPtr j = a1->join(a2, a1->column("c1")->eq(a2->column("c2")));
    j = j->join(a3, a2->column("c2")->eq(a3->column("c3")));
    compiler.compile(*j);

    EXPECT_EQ(compiler.compiled(), "t1 AS a1 JOIN t2 AS a2 ON a1.c1 = a2.c2 JOIN t3 AS a3 ON a2.c2 = a3.c3");
}

TEST_F(sql_Compiler_test, test_select) {
    SelectPtr select = Select::create();
    ColumnPtr column = t1->column("c1");
    ColumnPtr column2 = t1->column("c2");
    ColumnPtr column3 = t2->column("c3");
    select->from()->add(t1);
    select->from()->add(t2);
    select->what(column);
    select->what(column2);
    select->what(column3);
    select->where(column->lt(xpr.int64_(1)));
    String expected("SELECT t1.c1, t1.c2, t2.c3\nFROM t1, t2\nWHERE t1.c1 < :lit_0");
    compiler.compile(*select);
    EXPECT_EQ(compiler.compiled(), expected);
    EXPECT_EQ(bind(":lit_0"), Variant(1));
}

TEST_F(sql_Compiler_test, test_factory_or_) {
    ExpressionPtr e1 = t1->column("c1")->eq(xpr.int64_(1));
    ExpressionPtr e2 = t1->column("c1")->eq(xpr.int64_(2));
    ExpressionPtr e3 = xpr.or_(e1, e2);
    String expected("t1.c1 = :lit_0 OR t1.c1 = :lit_1");
    compiler.compile(*e3);
    EXPECT_EQ(expected, compiler.compiled());
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(2));
}

} // namespace sql
} // namespace brfc

