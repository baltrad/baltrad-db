#include <boost/test/unit_test.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/FromClause.hpp>
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
