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

#include <brfc/expr/Listcons.hpp>

#include <brfc/sql/DialectCompiler.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Select.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockDialect.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

struct sql_DialectCompiler_test: public testing::Test {
    sql_DialectCompiler_test()
            : dialect()
            , compiler(&dialect)
            , xpr() {
    }

    virtual void SetUp() {
        dialect.delegate_to_fake();
    }
    
    ::testing::NiceMock<MockDialect> dialect;
    DialectCompiler compiler;
    Factory xpr;
};

TEST_F(sql_DialectCompiler_test, test_simple) {
    Expression expr = xpr.lt(xpr.int64_(1), xpr.int64_(2));
    Expression out = Listcons().string("(1 < 2)").get();

    EXPECT_EQ(out, compiler.compile(expr));
}

TEST_F(sql_DialectCompiler_test, test_not) {
    Expression expr = xpr.not_(xpr.bool_(true));
    Expression out = Listcons().string("NOT true").get();

    EXPECT_EQ(out, compiler.compile(expr));
}

TEST_F(sql_DialectCompiler_test, test_between) {
    Expression expr = xpr.between(xpr.int64_(1), xpr.int64_(0), xpr.int64_(2));
    Expression out = Listcons().string("((0 <= 1) AND (1 <= 2))").get();

    EXPECT_EQ(out, compiler.compile(expr));
}

TEST_F(sql_DialectCompiler_test, test_string_literal) {
    Expression l = xpr.string("a string");
    Expression out = Listcons().string("'a string'").get();

    EXPECT_EQ(out, compiler.compile(l));
}

TEST_F(sql_DialectCompiler_test, test_column) {
    Expression c = xpr.column("t1", "c1");
    Expression out = Listcons().string("t1.c1").get();

    EXPECT_EQ(out, compiler.compile(c));
}

TEST_F(sql_DialectCompiler_test, test_bind) {
    Expression b = xpr.bind("b");
    Expression out = Listcons().symbol("bind").string("b").get();

    const Expression& q = compiler.compile(b);
    EXPECT_EQ(out, q);
}

TEST_F(sql_DialectCompiler_test, test_aliased_select) {
    Select subq;
    subq.from(xpr.table("t1"));
    subq.what(xpr.column("t1", "c1"));
    Expression a = xpr.alias(subq.expression(), "a");
    Select select;
    select.from(a);
    select.what(xpr.column("a", "c1"));
    Expression out = Listcons().string("SELECT a.c1 FROM (SELECT t1.c1 FROM t1) AS a").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_basic_join) {
    FromClause f(xpr.table("t1"));
    f.join(xpr.table("t2"), xpr.eq(xpr.column("t1", "c1"), xpr.column("t2", "c2")));
    Expression out = Listcons().string("FROM t1 JOIN t2 ON (t1.c1 = t2.c2)").get();

    EXPECT_EQ(out, compiler.compile(f.expression()));
}

TEST_F(sql_DialectCompiler_test, test_outerjoin) {
    FromClause f(xpr.table("t1"));
    f.outerjoin(xpr.table("t2"), xpr.eq(xpr.column("t1", "c1"), xpr.column("t2", "c2")));
    Expression out = Listcons().string("FROM t1 LEFT JOIN t2 ON (t1.c1 = t2.c2)").get();

    EXPECT_EQ(out, compiler.compile(f.expression()));
}

TEST_F(sql_DialectCompiler_test, test_join_with_alias) {
    FromClause f(xpr.table("t1"));
    f.join(
        xpr.alias(xpr.table("t2"), "a"),
        xpr.eq(xpr.column("t1", "c1"), xpr.column("a", "c2"))
    );
    Expression out = Listcons().string("FROM t1 JOIN t2 AS a ON (t1.c1 = a.c2)").get();

    EXPECT_EQ(out, compiler.compile(f.expression()));
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
    Expression out = Listcons().string("SELECT t1.c1, t1.c2, t2.c3 FROM t1 JOIN t2 ON (t1.c1 = t2.c2) WHERE (t1.c1 < 1)").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_select_group_by) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.append_group_by(xpr.column("t1", "c1"));
    select.append_group_by(xpr.column("t1", "c2"));
    Expression out = Listcons().string("SELECT t1.c1 FROM t1 GROUP BY t1.c1, t1.c2").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_select_order_by) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.what(xpr.column("t1", "c2"));
    select.append_order_by(xpr.column("t1", "c1"), Select::ASC);
    select.append_order_by(xpr.column("t1", "c3"), Select::DESC);
    Expression out = Listcons().string("SELECT t1.c1, t1.c2 FROM t1 ORDER BY t1.c1 ASC, t1.c3 DESC").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_select_limit) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.limit(1);
    Expression out = Listcons().string("SELECT t1.c1 FROM t1 LIMIT 1").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_select_offset) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.offset(2);
    Expression out = Listcons().string("SELECT t1.c1 FROM t1 OFFSET 2").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_select_offset_with_limit) {
    Select select;
    select.from(xpr.table("t1"));
    select.what(xpr.column("t1", "c1"));
    select.limit(1);
    select.offset(2);
    Expression out = Listcons().string("SELECT t1.c1 FROM t1 LIMIT 1 OFFSET 2").get();

    EXPECT_EQ(out, compiler.compile(select.expression()));
}

TEST_F(sql_DialectCompiler_test, test_insert) {
    Insert insert("t1");
    insert.value("c1", xpr.int64_(1));
    insert.value("c2", xpr.int64_(2));
    insert.returning(xpr.column("t1", "c3"));
    Expression out = Listcons().string("INSERT INTO t1 (c1, c2) VALUES (1, 2) RETURNING t1.c3").get();

    EXPECT_EQ(out, compiler.compile(insert.expression()));
}

TEST_F(sql_DialectCompiler_test, test_factory_or_) {
    Expression e1 = xpr.eq(xpr.column("t1", "c1"), xpr.int64_(1));
    Expression e2 = xpr.eq(xpr.column("t1", "c1"), xpr.int64_(2));
    Expression e3 = xpr.or_(e1, e2);
    Expression out = Listcons().string("((t1.c1 = 1) OR (t1.c1 = 2))").get();

    EXPECT_EQ(out, compiler.compile(e3));
}

TEST_F(sql_DialectCompiler_test, test_proc_binop) {
    DialectCompiler::binop proc("op");
    
    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("(").int64(1)
                               .string(" op ")
                               .int64(2).string(")").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_like) {
    DialectCompiler::like proc;

    Expression in = Listcons().string("qwe").string("f?ob*").get();
    Expression out = Listcons().string("qwe").string(" LIKE f_ob%").get();
    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_unaryop) {
    DialectCompiler::unaryop proc("op");

    Expression in = Listcons().int64(1).get();
    Expression out = Listcons().string("op ").int64(1).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_bind) {
    DialectCompiler::bind proc;

    Expression in = Listcons().string("bindname").get();
    Expression out = Listcons().symbol("bind").string("bindname").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_column) {
    DialectCompiler::column proc;

    Expression in = Listcons().string("table").string("column").get();
    Expression out("table.column");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_function) {
    DialectCompiler::function proc("func");

    Expression in = Listcons().int64(1).int64(2).int64(3).get();
    Expression out = Listcons().string("func(")
                               .int64(1).string(", ")
                               .int64(2).string(", ")
                               .int64(3).string(")").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_alias) {
    DialectCompiler::alias proc;

    Expression in = Listcons().string("foo").string("bar").get();
    Expression out = Listcons().string("foo").string(" AS bar").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_join) {
    DialectCompiler::join proc("JOIN");

    Expression in = Listcons().string("table").bool_(true).get();
    Expression out = Listcons().string("JOIN ").string("table").string(" ON ").bool_(true).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_literal) {
    DialectCompiler::literal proc(dialect);

    Expression in(1);
    Expression out("1");

    EXPECT_CALL(dialect, do_literal_to_string(Expression(1)))
        .WillOnce(Return("1"));
    
    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_label) {
    DialectCompiler::label proc;

    Expression in = Listcons().int64(1).string("bar").get();
    Expression out = Listcons().int64(1).string(" AS bar").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_distinct) {
    DialectCompiler::distinct proc;

    Expression in;
    Expression out("DISTINCT");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_select_columns) {
    DialectCompiler::select_columns proc;

    Expression in = Listcons().int64(1).int64(2).int64(3).get();
    Expression out = Listcons().int64(1).string(", ")
                               .int64(2).string(", ")
                               .int64(3).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_from_clause) {
    DialectCompiler::from_clause proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("FROM ")
                               .int64(1).string(" ")
                               .int64(2).get();
    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_where_clause) {
    DialectCompiler::where_clause proc;

    Expression in = Listcons().bool_(true).get();
    Expression out = Listcons().string("WHERE ").bool_(true).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_group_by) {
    DialectCompiler::group_by proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("GROUP BY ").int64(1).string(", ").int64(2).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_order_by) {
    DialectCompiler::order_by proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("ORDER BY ").int64(1).string(", ").int64(2).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_order_dir) {
    DialectCompiler::order_dir proc("ASC");

    Expression in = Listcons().int64(1).get();
    Expression out = Listcons().int64(1).string(" ASC").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_limit) {
    DialectCompiler::limit proc;

    Expression in = Listcons().string("1").get();
    Expression out("LIMIT 1");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_offset) {
    DialectCompiler::offset proc;

    Expression in = Listcons().string("1").get();
    Expression out("OFFSET 1");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_select) {
    DialectCompiler::select proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("SELECT ").int64(1).string(" ").int64(2).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_insert_columns) {
    DialectCompiler::insert_columns proc;

    Expression in = Listcons().string("col1").string("col2").get();
    Expression out("(col1, col2)");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_insert_values) {
    DialectCompiler::insert_values proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("VALUES (").int64(1).string(", ")
                               .int64(2).string(")").get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_returning) {
    DialectCompiler::returning proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("RETURNING ").int64(1)
                               .string(", ").int64(2).get();
    
    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_insert) {
    DialectCompiler::insert proc;

    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().string("INSERT INTO ").int64(1).string(" ").int64(2).get();

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_proc_table) {
    DialectCompiler::table proc;

    Expression in = Listcons().string("table").get();
    Expression out("table");

    EXPECT_EQ(out, proc(in));
}

TEST_F(sql_DialectCompiler_test, test_compact_str_stringlist) {
    Expression in = Listcons().string("foo").string("bar").string("baz").get();
    Expression out = Listcons().string("foobarbaz").get();

    EXPECT_EQ(out, compiler.compact_str(in));
}

TEST_F(sql_DialectCompiler_test, test_compact_str_nested_stringlist) {
    Expression nest = Listcons().string("qwe").string("asd").get();
    Expression in = Listcons().string("foo").string("bar")
                              .append(nest).string("baz").get();
    Expression out = Listcons().string("foobarqweasdbaz").get();

    EXPECT_EQ(out, compiler.compact_str(in));
}

TEST_F(sql_DialectCompiler_test, test_compact_str_mixedlist) {
    Expression in = Listcons().string("foo").string("bar")
                              .int64(1).string("baz").int64(2).get();
    Expression out = Listcons().string("foobar").int64(1)
                               .string("baz").int64(2).get();
    EXPECT_EQ(out, compiler.compact_str(in));
}

TEST_F(sql_DialectCompiler_test, test_compact_str_nested_mixeedlist) {
    Expression nest = Listcons().string("qwe").int64(10).string("asd").get();
    Expression in = Listcons().string("foo").string("bar").append(nest)
                              .int64(1).string("baz").int64(2).get();
    Expression out = Listcons().string("foobarqwe").int64(10)
                               .string("asd").int64(1)
                               .string("baz").int64(2).get();
    
    EXPECT_EQ(out, compiler.compact_str(in));
}

} // namespace sql
} // namespace brfc
