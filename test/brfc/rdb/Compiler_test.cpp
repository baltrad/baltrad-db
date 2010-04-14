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

#include <QtCore/QString>


#include <brfc/Variant.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/rdb/Alias.hpp>
#include <brfc/rdb/Column.hpp>
#include <brfc/rdb/FromClause.hpp>
#include <brfc/rdb/Join.hpp>
#include <brfc/rdb/Select.hpp>
#include <brfc/rdb/Table.hpp>

#include <brfc/rdb/Compiler.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

using namespace expr;

struct rdb_Compiler_test: public testing::Test {
    Variant bind(const QString& key) const {
        Compiler::BindMap::const_iterator i = compiler.binds().find(key);
        return i != compiler.binds().end() ? i->second : Variant();
    }

    Compiler compiler;
    Factory xpr;
};

TEST_F(rdb_Compiler_test, test_simple) {
    ExpressionPtr expr = xpr.integer(1)->lt(xpr.integer(2));
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), ":lit_0 < :lit_1");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(2));
}

TEST_F(rdb_Compiler_test, test_between) {
    ExpressionPtr expr = xpr.integer(1)->between(xpr.integer(0), xpr.integer(2));
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), ":lit_0 >= :lit_1 AND :lit_2 <= :lit_3");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(0));
    EXPECT_EQ(bind(":lit_2"), Variant(1));
    EXPECT_EQ(bind(":lit_3"), Variant(2));
}

TEST_F(rdb_Compiler_test, test_string_literal) {
    LiteralPtr l = xpr.string("a string");
    compiler.compile(*l);
    EXPECT_EQ(compiler.compiled(), ":lit_0");
    EXPECT_EQ(bind(":lit_0"), Variant("a string"));
}

TEST_F(rdb_Compiler_test, test_parentheses) {
    ExpressionPtr expr = xpr.integer(1)->parentheses();
    compiler.compile(*expr);
    EXPECT_EQ(compiler.compiled(), "(:lit_0)");
    EXPECT_EQ(bind(":lit_0"), Variant(1));
}

TEST_F(rdb_Compiler_test, test_column) {
    ColumnPtr c = Table::create("table_name")->column("column_name");
    compiler.compile(*c);
    EXPECT_EQ(compiler.compiled(), "table_name.column_name");
}

TEST_F(rdb_Compiler_test, test_aliased_table_column) {
    ColumnPtr c = Table::create("table_name")->alias("table_alias")->column("column_name");
    compiler.compile(*c);
    EXPECT_EQ(compiler.compiled(), "table_alias.column_name");
}

TEST_F(rdb_Compiler_test, test_basic_join) {
    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 JOIN t2 ON t1.c1 = t2.c2");
}

TEST_F(rdb_Compiler_test, test_outerjoin) {
    SelectablePtr t1 = Table::create("t1");
    SelectablePtr t2 = Table::create("t2");
    JoinPtr j = t1->outerjoin(t2, t1->column("c1")->eq(t2->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 LEFT JOIN t2 ON t1.c1 = t2.c2");
}



TEST_F(rdb_Compiler_test, test_join_with_alias) {
    TablePtr t1 = Table::create("t1");
    SelectablePtr a = Table::create("t2")->alias("a");
    JoinPtr j = t1->join(a, t1->column("c1")->eq(a->column("c2")));
    compiler.compile(*j);
    EXPECT_EQ(compiler.compiled(), "t1 JOIN t2 AS a ON t1.c1 = a.c2");
}

TEST_F(rdb_Compiler_test, test_join_with_multiple_aliases) {
    SelectablePtr a1 = Table::create("t1")->alias("a1");
    SelectablePtr a2 = Table::create("t2")->alias("a2");
    SelectablePtr a3 = Table::create("t3")->alias("a3");

    JoinPtr j = a1->join(a2, a1->column("c1")->eq(a2->column("c2")));
    j = j->join(a3, a2->column("c2")->eq(a3->column("c3")));
    compiler.compile(*j);

    EXPECT_EQ(compiler.compiled(), "t1 AS a1 JOIN t2 AS a2 ON a1.c1 = a2.c2 JOIN t3 AS a3 ON a2.c2 = a3.c3");
}

TEST_F(rdb_Compiler_test, test_select) {
    SelectPtr select = Select::create();
    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    ColumnPtr column = t1->column("c1");
    ColumnPtr column2 = t1->column("c2");
    ColumnPtr column3 = t2->column("c3");
    select->from()->add(t1);
    select->from()->add(t2);
    select->what(column);
    select->what(column2);
    select->what(column3);
    select->where(column->lt(xpr.integer(1)));
    QString expected("SELECT t1.c1, t1.c2, t2.c3\nFROM t1, t2\nWHERE t1.c1 < :lit_0");
    compiler.compile(*select);
    EXPECT_EQ(compiler.compiled(), expected);
    EXPECT_EQ(bind(":lit_0"), Variant(1));
}

TEST_F(rdb_Compiler_test, test_factory_or_) {
    TablePtr t = Table::create("t");
    ExpressionPtr e1 = t->column("c")->eq(xpr.integer(1));
    ExpressionPtr e2 = t->column("c")->eq(xpr.integer(2));
    ExpressionPtr e3 = xpr.or_(e1, e2);
    QString expected("t.c = :lit_0 OR t.c = :lit_1");
    compiler.compile(*e3);
    EXPECT_EQ(expected, compiler.compiled());
    EXPECT_EQ(bind(":lit_0"), Variant(1));
    EXPECT_EQ(bind(":lit_1"), Variant(2));
}

} // namespace rdb
} // namespace brfc

