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
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

class sql_Alias_test : public ::testing::Test {
  public:
    sql_Alias_test()
            : t1(Table::create("t1")) {
    }
    
    void SetUp() {
        t1->add_column(Column::create("c1"));
        t1->add_column(Column::create("c2"));
    }

    TablePtr t1, t2;
};

TEST_F(sql_Alias_test, test_column) {
    AliasPtr a = t1->alias("a");
    
    ColumnPtr c;
    EXPECT_THROW(a->column("d1"), lookup_error);
    EXPECT_NO_THROW(c = a->column("c1"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), a);
}

} // namespace sql
} // namespace brfc
