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
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

class sql_Join_test : public ::testing::Test {
  public:
    sql_Join_test()
            : t1(Table::create("t1"))
            , t2(Table::create("t2")) {
    }
    
    void SetUp() {
        t1->add_column("c1");
        t1->add_column("c2");
        t2->add_column("d1");
        t2->add_column("d2");
    }

    TablePtr t1, t2;
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

} // namespace sql
} // namespace brfc
