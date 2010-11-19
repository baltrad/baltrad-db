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

#include <brfc/Variant.hpp>

#include <brfc/sql/MockDialect.hpp>

namespace brfc {
namespace sql {

class sql_Dialect_test : public testing::Test {
  public:
    sql_Dialect_test()
        : dialect() {
    }

    ::testing::NiceMock<MockDialect> dialect;
};

TEST_F(sql_Dialect_test, test_variant_to_string_string) {
    EXPECT_EQ("'qweqwe'", dialect.variant_to_string(Variant("qweqwe")));
}

TEST_F(sql_Dialect_test, test_variant_to_string_integer) {
    EXPECT_EQ("1", dialect.variant_to_string(Variant(1)));
}

TEST_F(sql_Dialect_test, test_variant_to_string_float) {
    EXPECT_EQ("0.5", dialect.variant_to_string(Variant(0.5)));
}

TEST_F(sql_Dialect_test, test_variant_to_string_date) {
    EXPECT_EQ("'2001-05-01'", dialect.variant_to_string(Variant(Date(2001, 5, 1))));
}

TEST_F(sql_Dialect_test, test_variant_to_string_time) {
    EXPECT_EQ("'13:05:59.001'", dialect.variant_to_string(Variant(Time(13, 5, 59, 1))));
}

TEST_F(sql_Dialect_test, test_variant_to_string_null) {
    EXPECT_EQ("NULL", dialect.variant_to_string(Variant()));
}

} // namespace sql
} // namespace brfc
