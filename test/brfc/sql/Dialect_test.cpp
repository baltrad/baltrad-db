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

#include <brfc/expr/Expression.hpp>

#include <brfc/sql/MockDialect.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_Dialect_test : public testing::Test {
  public:
    sql_Dialect_test()
        : dialect() {
    }

    virtual void SetUp() {
        dialect.delegate_to_fake();
    }

    ::testing::NiceMock<MockDialect> dialect;
};


TEST_F(sql_Dialect_test, test_ansi_sql_interval_str_nulls) {
    TimeDelta delta;
    EXPECT_EQ("0 00:00:00.000", ansi_sql_interval_str(delta));
}

TEST_F(sql_Dialect_test, test_ansi_sql_interval_str_single_digit) {
    TimeDelta delta;
    delta.add_days(10);
    delta.add_hours(5);
    delta.add_minutes(6);
    delta.add_seconds(7);
    delta.add_msecs(8);

    EXPECT_EQ("10 05:06:07.008", ansi_sql_interval_str(delta));
}

TEST_F(sql_Dialect_test, test_ansi_sql_interval_str_maxwidth) {
    TimeDelta delta;
    delta.add_days(10);
    delta.add_hours(20);
    delta.add_minutes(30);
    delta.add_seconds(40);
    delta.add_msecs(500);

    EXPECT_EQ("10 20:30:40.500", ansi_sql_interval_str(delta));
}

TEST_F(sql_Dialect_test, test_literal_to_string_string) {
    EXPECT_CALL(dialect, do_escape("qweqwe"))
        .WillOnce(Return("asdasd"));
    EXPECT_EQ("'asdasd'", dialect.literal_to_string(Expression("qweqwe")));
}

TEST_F(sql_Dialect_test, test_literal_to_string_integer) {
    EXPECT_EQ("1", dialect.literal_to_string(Expression(1)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_float) {
    EXPECT_EQ("0.5", dialect.literal_to_string(Expression(0.5)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_date) {
    Date d(2001, 5, 1);
    EXPECT_EQ("'2001-05-01'", dialect.literal_to_string(Expression(d)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_time) {
    Time t(13, 5, 59, 1);
    EXPECT_EQ("'13:05:59.001000'", dialect.literal_to_string(Expression(t)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_datetime) {
    DateTime dt(2001, 5, 1, 13, 5, 59, 1);
    EXPECT_EQ("'2001-05-01T13:05:59.001000'", dialect.literal_to_string(Expression(dt)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_timedelta) {
    TimeDelta delta;
    delta.add_days(1);
    delta.add_hours(23);
    delta.add_minutes(34);
    delta.add_seconds(45);
    delta.add_msecs(567);
    EXPECT_EQ("INTERVAL '1 23:34:45.567'", dialect.literal_to_string(Expression(delta)));
}

TEST_F(sql_Dialect_test, test_literal_to_string_null) {
    EXPECT_EQ("NULL", dialect.literal_to_string(Expression()));
}

} // namespace sql
} // namespace brfc
