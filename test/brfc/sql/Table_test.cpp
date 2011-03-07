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

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

TEST(sql_Table_test, test_column) {
    TablePtr t = Table::create("t");

    EXPECT_THROW(t->column("c"), lookup_error);

    ColumnPtr c = t->add_column("c");
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), t);
    EXPECT_EQ(t->column("c"), c);

    EXPECT_THROW(t->add_column("c"), duplicate_entry);

}

TEST(sql_Table_test, test_fk_columns) {
    TablePtr t1 = Table::create("t1");
    t1->add_column("c1");
    t1->add_column("c2");
    
    TablePtr t2 = Table::create("t2");
    t2->add_column("d1");
    t2->add_column("d2");
    t2->add_column("d3");

    t2->column("d1")->references(t1->column("c1"));
    t2->column("d2")->references(t1->column("c2"));

    const std::vector<ColumnPtr> fkcols = t2->fk_columns();
    EXPECT_EQ((size_t)2, fkcols.size());
    EXPECT_TRUE(std::find(fkcols.begin(), fkcols.end(), t2->column("d1")) != fkcols.end());
    EXPECT_TRUE(std::find(fkcols.begin(), fkcols.end(), t2->column("d2")) != fkcols.end());
}

} // namespace sql
} // namespace brfc
