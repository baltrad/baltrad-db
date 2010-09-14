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

#include <brfc/exceptions.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/DialectCompiler.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Table.hpp>

#include "MockDialect.hpp"

namespace brfc {
namespace sql {

class sql_Join_test : public ::testing::Test {
  public:
    sql_Join_test()
            : dialect()
            , compiler(&dialect)
            , t1(Table::create("t1"))
            , t2(Table::create("t2"))
            , t3(Table::create("t3")) {
    }
    
    void SetUp() {
        t1->add_column("c1");
        t1->add_column("c2");
        t2->add_column("d1");
        t2->add_column("d2");
        t3->add_column("e1");
        t3->add_column("e2");
    }
    
    ::testing::NiceMock<MockDialect> dialect;
    DialectCompiler compiler;
    TablePtr t1, t2, t3;
};

TEST_F(sql_Join_test, test_column) {
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("d1")));

    EXPECT_THROW(j->column("e1"), lookup_error);
    ColumnPtr c;
    EXPECT_NO_THROW(c = j->column("c1"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), t1);

    EXPECT_NO_THROW(c = j->column("d1"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), t2);
}

TEST_F(sql_Join_test, test_colums) {
    JoinPtr j = t1->join(t2, t1->column("c1")->eq(t2->column("d1")));

    const std::vector<ColumnPtr> cols = j->columns();
    EXPECT_EQ((size_t)4, cols.size());
    EXPECT_TRUE(cols.end() != std::find(cols.begin(), cols.end(), t1->column("c1")));
    EXPECT_TRUE(cols.end() != std::find(cols.begin(), cols.end(), t1->column("c2")));
    EXPECT_TRUE(cols.end() != std::find(cols.begin(), cols.end(), t2->column("d1")));
    EXPECT_TRUE(cols.end() != std::find(cols.begin(), cols.end(), t2->column("d2")));
}

TEST_F(sql_Join_test, test_contains) {
    JoinPtr j = t1->crossjoin(t2)->crossjoin(t3);
    EXPECT_TRUE(j->contains(t1));
    EXPECT_TRUE(j->contains(t2));
    EXPECT_TRUE(j->contains(t3));
    EXPECT_FALSE(j->contains(t1->alias("a")));
}

TEST_F(sql_Join_test, test_auto_condition) {
    t1->column("c1")->references(t2->column("d1"));
    JoinPtr j;
    ASSERT_NO_THROW(j = t1->join(t2));
    ASSERT_TRUE(j->condition());
    EXPECT_EQ("t1.c1 = t2.d1",
              compiler.compile(*j->condition()).statement());
}

TEST_F(sql_Join_test, test_auto_condition_on_chain) {
    t1->column("c1")->references(t2->column("d1"));
    t2->column("d2")->references(t3->column("e2"));
    JoinPtr j;
    ASSERT_NO_THROW(j = t1->join(t2)->join(t3));
    ASSERT_TRUE(j->condition());
    EXPECT_EQ("t2.d2 = t3.e2",
              compiler.compile(*j->condition()).statement());
}

TEST_F(sql_Join_test, test_auto_condition_on_alias) {
    t1->column("c2")->references(t2->column("d2"));
    AliasPtr a1 = t1->alias("a1");
    AliasPtr a2 = t2->alias("a2");

    JoinPtr j;
    ASSERT_NO_THROW(j = a1->join(a2));
    BinaryOperatorPtr cond = 
        dynamic_pointer_cast<BinaryOperator>(j->condition());
    ASSERT_TRUE(cond);
    EXPECT_EQ("a1.c2 = a2.d2",
              compiler.compile(*j->condition()).statement());
}

} // namespace sql
} // namespace brfc
