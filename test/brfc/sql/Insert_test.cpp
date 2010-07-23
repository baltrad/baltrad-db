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
#include <brfc/Variant.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

TEST(sql_Insert_test, test_value) {
    TablePtr t = Table::create("t");
    t->add_column("c");

    InsertPtr stmt = Insert::create(t);
    LiteralPtr val = Literal::create(Variant(1));

    EXPECT_THROW(stmt->value("nxcol", val), lookup_error);

    stmt->value("c", val);
    EXPECT_EQ(stmt->values().size(), 1);

    EXPECT_THROW(stmt->value("c", val), duplicate_entry);
}

} // namespace sql
} // namespace brfc
