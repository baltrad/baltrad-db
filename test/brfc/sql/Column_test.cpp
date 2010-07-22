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

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

TEST(sql_Column_test, test_rebase) {
    TablePtr t = Table::create("t");
    ColumnPtr c = Column::create("c", t);
    ColumnPtr d = Column::create("d", t);
    d->references(c);

    AliasPtr a = t->alias("a");

    ColumnPtr r = d->rebase(a);

    EXPECT_EQ(a, r->selectable());
    EXPECT_EQ(c, r->references());
}

} // namespace sql 
} // namespace brfc
