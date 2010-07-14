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

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

TEST(sql_Table_test, test_column) {
    TablePtr t = Table::create("t");

    EXPECT_THROW(t->column("c"), lookup_error);

    ColumnPtr c = Column::create("c");
    t->add_column(c);
    EXPECT_EQ(c->selectable(), t);
    EXPECT_EQ(t->column("c"), c);

    EXPECT_THROW(t->add_column(c), value_error);
    EXPECT_THROW(t->add_column(Column::create("c")), duplicate_entry);

}

} // namespace sql
} // namespace brfc
