#include <boost/test/unit_test.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Join.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/Table.hpp>
#include <brfc/expr/Select.hpp>

#include "common.hpp"

using namespace brfc::expr;

struct Fixture {
    QVariant bind(const QString& key) const {
        Compiler::BindMap::const_iterator i = compiler.binds().find(key);
        return i != compiler.binds().end() ? i->second : QVariant();
    }

    Compiler compiler;
    Factory xpr;
};

BOOST_FIXTURE_TEST_SUITE(Expression_test, Fixture)

BOOST_AUTO_TEST_CASE(test_simple) {
    ExpressionPtr expr = xpr.integer(1)->lt(xpr.integer(2));
    compiler.compile(*expr);
    BOOST_CHECK_EQUAL(compiler.compiled(), ":lit_0 < :lit_1");
    BOOST_CHECK_EQUAL(bind(":lit_0"), 1);
    BOOST_CHECK_EQUAL(bind(":lit_1"), 2);
}

BOOST_AUTO_TEST_CASE(test_between) {
    ExpressionPtr expr = xpr.integer(1)->between(xpr.integer(0), xpr.integer(2));
    compiler.compile(*expr);
    BOOST_CHECK_EQUAL(compiler.compiled(), ":lit_0 >= :lit_1 AND :lit_2 <= :lit_3");
    BOOST_CHECK_EQUAL(bind(":lit_0"), 1);
    BOOST_CHECK_EQUAL(bind(":lit_1"), 0);
    BOOST_CHECK_EQUAL(bind(":lit_2"), 1);
    BOOST_CHECK_EQUAL(bind(":lit_3"), 2);
}

BOOST_AUTO_TEST_CASE(test_string_literal) {
    LiteralPtr l = xpr.string("a string");
    compiler.compile(*l);
    BOOST_CHECK_EQUAL(compiler.compiled(), ":lit_0");
    BOOST_CHECK_EQUAL(bind(":lit_0"), "a string");
}

BOOST_AUTO_TEST_CASE(test_parentheses) {
    ExpressionPtr expr = xpr.integer(1)->parentheses();
    compiler.compile(*expr);
    BOOST_CHECK_EQUAL(compiler.compiled(), "(:lit_0)");
    BOOST_CHECK_EQUAL(bind(":lit_0"), 1);
}

BOOST_AUTO_TEST_CASE(test_column) {
    ColumnPtr c = Table::create("table_name")->column("column_name");
    compiler.compile(*c);
    BOOST_CHECK_EQUAL(compiler.compiled(), "table_name.column_name");
}

BOOST_AUTO_TEST_CASE(test_aliased_table_column) {
    ColumnPtr c = Table::create("table_name")->alias("table_alias")->column("column_name");
    compiler.compile(*c);
    BOOST_CHECK_EQUAL(compiler.compiled(), "table_alias.column_name");
}

BOOST_AUTO_TEST_CASE(test_basic_join) {
    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("c2")));
    compiler.compile(*j);
    BOOST_CHECK_EQUAL(compiler.compiled(), "t1 JOIN t2 ON t1.c1 = t2.c2");
}

BOOST_AUTO_TEST_CASE(test_join_with_alias) {
    TablePtr t1 = Table::create("t1");
    SelectablePtr a = Table::create("t2")->alias("a");
    JoinPtr j = t1->join(a, t1->column("c1")->eq(a->column("c2")));
    compiler.compile(*j);
    BOOST_CHECK_EQUAL(compiler.compiled(), "t1 JOIN t2 AS a ON t1.c1 = a.c2");
}

BOOST_AUTO_TEST_CASE(test_join_with_multiple_aliases) {
    SelectablePtr a1 = Table::create("t1")->alias("a1");
    SelectablePtr a2 = Table::create("t2")->alias("a2");
    SelectablePtr a3 = Table::create("t3")->alias("a3");

    JoinPtr j = a1->join(a2, a1->column("c1")->eq(a2->column("c2")));
    j = j->join(a3, a2->column("c2")->eq(a3->column("c3")));
    compiler.compile(*j);

    BOOST_CHECK_EQUAL(compiler.compiled(), "t1 AS a1 JOIN t2 AS a2 ON a1.c1 = a2.c2 JOIN t3 AS a3 ON a2.c2 = a3.c3");
}

BOOST_AUTO_TEST_CASE(test_join_contains) {
    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    AliasPtr t2a = t2->alias("t2a");
    TablePtr t3 = Table::create("t3");
    JoinPtr j = t1->join(t2a, xpr.integer(1))->join(t3, xpr.integer(1));
    BOOST_CHECK(j->contains(t1));
    BOOST_CHECK(not j->contains(t2));
    BOOST_CHECK(j->contains(t2a));
    BOOST_CHECK(j->contains(t3));
}

BOOST_AUTO_TEST_CASE(test_select) {
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
    std::string expected = "SELECT t1.c1, t1.c2, t2.c3\nFROM t1, t2\nWHERE t1.c1 < :lit_0";
    compiler.compile(*select);
    BOOST_CHECK_EQUAL(compiler.compiled(), expected);
    BOOST_CHECK_EQUAL(bind(":lit_0"), 1);
}

BOOST_AUTO_TEST_SUITE_END()
