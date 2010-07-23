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

class sql_Column_test : public ::testing::Test {
  public:
    sql_Column_test()
            : t(Table::create("t")) {
    }

    TablePtr t;
};

TEST_F(sql_Column_test, test_references) {
    ColumnPtr c1 = Column::create("c1", t);
    ColumnPtr c2 = Column::create("c2", t);

    EXPECT_NO_THROW(c1->references(c2));
    EXPECT_THROW(c1->references(c2), value_error);
}

TEST_F(sql_Column_test, test_proxy) {
    ColumnPtr c = Column::create("c", t);
    ColumnPtr d = Column::create("d", t);
    d->references(c);

    AliasPtr a = t->alias("a");

    ColumnPtr r = d->proxy(a);

    EXPECT_EQ(a, r->selectable());
    EXPECT_EQ(c, r->references());
}

} // namespace sql 
} // namespace brfc
