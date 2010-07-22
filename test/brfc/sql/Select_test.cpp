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

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/FromClause.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

class sql_Select_test : public ::testing::Test {
  public:
    sql_Select_test()
            : t1(Table::create("t1"))
            , t2(Table::create("t2")) {
    }
    
    void SetUp() {
        t1->add_column(Column::create("c1"));
        t1->add_column(Column::create("c2"));
        t2->add_column(Column::create("d1"));
        t2->add_column(Column::create("d2"));
    }

    TablePtr t1, t2;
};

TEST_F(sql_Select_test, test_column) {
    SelectPtr s = Select::create();
    EXPECT_THROW(s->column("c1"), lookup_error);
    s->from()->add(t1);
    s->what(t1->column("c1"));
    
    ColumnPtr c;
    EXPECT_NO_THROW(c = s->column("c1"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), s);
}

TEST_F(sql_Select_test, test_column_labeled_expr) {
    SelectPtr s = Select::create();
    s->what(t1->column("c1")->label("l"));
    s->from()->add(t1);

    EXPECT_THROW(s->column("c1"), lookup_error);

    ColumnPtr c;
    EXPECT_NO_THROW(c = s->column("l"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), s);
}

} // namespace sql
} // namespace brfc
