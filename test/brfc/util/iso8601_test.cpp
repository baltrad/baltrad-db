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

#include <brfc/util/iso8601.hpp>

namespace brfc {
namespace iso8601 {

TEST(iso8601_test, test_parse_date) {
    date d;
    EXPECT_TRUE(parse_date("20000102", d));
    EXPECT_EQ(2000, d.year);
    EXPECT_EQ(1u, d.month);
    EXPECT_EQ(2u, d.day);
}

TEST(iso8601_test, test_parse_date_extended) {
    date d;
    EXPECT_TRUE(parse_date("2000-01-02", d));
    EXPECT_EQ(2000, d.year);
    EXPECT_EQ(1u, d.month);
    EXPECT_EQ(2u, d.day);
}

TEST(iso8601_test, test_parse_date_invalid) {
    date d;
    EXPECT_FALSE(parse_date("", d));
    EXPECT_FALSE(parse_date("asd", d));
    EXPECT_FALSE(parse_date("2000102", d));
    EXPECT_FALSE(parse_date("20000001", d));
    EXPECT_FALSE(parse_date("20001301", d));
    EXPECT_FALSE(parse_date("20000100", d));
    EXPECT_FALSE(parse_date("20000132", d));
    EXPECT_FALSE(parse_date("2000-00-01", d));
    EXPECT_FALSE(parse_date("2000-13-01", d));
    EXPECT_FALSE(parse_date("2000-01-00", d));
    EXPECT_FALSE(parse_date("2000-01-32", d));
}

TEST(iso8601_test, test_parse_time) {
    time t;
    EXPECT_TRUE(parse_time("010203", t));
    EXPECT_EQ(1u, t.hour);
    EXPECT_EQ(2u, t.minute);
    EXPECT_EQ(3u, t.second);
    EXPECT_EQ(0u, t.usec);
}

TEST(iso8601_test, test_parse_time_usec_period) {
    time t;
    EXPECT_TRUE(parse_time("111213.123456789", t));
    EXPECT_EQ(11u, t.hour);
    EXPECT_EQ(12u, t.minute);
    EXPECT_EQ(13u, t.second);
    EXPECT_EQ(123456u, t.usec);
}

TEST(iso8601_test, test_parse_time_usec_comma) {
    time t;
    EXPECT_TRUE(parse_time("203040,00123456789", t));
    EXPECT_EQ(20u, t.hour);
    EXPECT_EQ(30u, t.minute);
    EXPECT_EQ(40u, t.second);
    EXPECT_EQ(1234u, t.usec);
}

TEST(iso8601_test, test_parse_time_extended) {
    time t;
    EXPECT_TRUE(parse_time("01:02:03", t));
    EXPECT_EQ(1u, t.hour);
    EXPECT_EQ(2u, t.minute);
    EXPECT_EQ(3u, t.second);
    EXPECT_EQ(0u, t.usec);
}

TEST(iso8601_test, test_parse_time_extended_usec_period) {
    time t;
    EXPECT_TRUE(parse_time("11:12:13.456", t));
    EXPECT_EQ(11u, t.hour);
    EXPECT_EQ(12u, t.minute);
    EXPECT_EQ(13u, t.second);
    EXPECT_EQ(456000u, t.usec);
}

TEST(iso8601_test, test_parse_time_extended_usec_comma) {
    time t;
    EXPECT_TRUE(parse_time("20:30:40,016", t));
    EXPECT_EQ(20u, t.hour);
    EXPECT_EQ(30u, t.minute);
    EXPECT_EQ(40u, t.second);
    EXPECT_EQ(16000u, t.usec);
}

TEST(iso8601_test, test_parser_time_invalid) {
    time t;
    EXPECT_FALSE(parse_time("", t));
    EXPECT_FALSE(parse_time("asd", t));
    EXPECT_FALSE(parse_time("01020304", t));
    EXPECT_FALSE(parse_time("01020", t));
    EXPECT_FALSE(parse_time("010203.,123", t));
    EXPECT_FALSE(parse_time("010203,123,123", t));
    EXPECT_FALSE(parse_time("01:02:0304", t));
    EXPECT_FALSE(parse_time("01:02:0", t));
    EXPECT_FALSE(parse_time("01:02:03.,123", t));
    EXPECT_FALSE(parse_time("01:02:03,123,123", t));
}

TEST(iso8601_test, test_parse_datetime) {
    datetime dt;
    EXPECT_TRUE(parse_datetime("20000102T030405,06", dt));
    EXPECT_EQ(2000, dt.date.year);
    EXPECT_EQ(1u, dt.date.month);
    EXPECT_EQ(2u, dt.date.day);
    EXPECT_EQ(3u, dt.time.hour);
    EXPECT_EQ(4u, dt.time.minute);
    EXPECT_EQ(5u, dt.time.second);
    EXPECT_EQ(60000u, dt.time.usec);
}

TEST(iso8601_test, test_parse_datetime_space_separated) {
    datetime dt;
    EXPECT_TRUE(parse_datetime("20000102 030405,06", dt));
    EXPECT_EQ(2000, dt.date.year);
    EXPECT_EQ(1u, dt.date.month);
    EXPECT_EQ(2u, dt.date.day);
    EXPECT_EQ(3u, dt.time.hour);
    EXPECT_EQ(4u, dt.time.minute);
    EXPECT_EQ(5u, dt.time.second);
    EXPECT_EQ(60000u, dt.time.usec);
}

TEST(iso8601_test, test_parse_datetime_extended) {
    datetime dt;
    EXPECT_TRUE(parse_datetime("2000-01-02T03:04:05,000000001", dt));
    EXPECT_EQ(2000, dt.date.year);
    EXPECT_EQ(1u, dt.date.month);
    EXPECT_EQ(2u, dt.date.day);
    EXPECT_EQ(3u, dt.time.hour);
    EXPECT_EQ(4u, dt.time.minute);
    EXPECT_EQ(5u, dt.time.second);
    EXPECT_EQ(0u, dt.time.usec);
}

TEST(iso8601_test, test_date_to_string) {
    date d(2000, 1, 2);
    EXPECT_EQ("20000102", to_string(d));
}

TEST(iso8601_test, test_date_to_string_extended) {
    date d(2000, 1, 2);
    EXPECT_EQ("2000-01-02", to_string(d, format::EXTENDED));
}

TEST(iso8601_test, test_time_to_string) {
    time t(1,2,3,4);
    EXPECT_EQ("010203.000004", to_string(t));
}

TEST(iso8601_test, test_time_to_string_extended) {
    time t(1,2,3,4);
    EXPECT_EQ("01:02:03.000004", to_string(t, format::EXTENDED));
}

TEST(iso8601_test, test_time_to_string_zero_msec) {
    time t(1,2,3);
    EXPECT_EQ("010203", to_string(t));
}

TEST(iso8601_test, test_time_to_string_zero_msec_extended) {
    time t(1,2,3);
    EXPECT_EQ("01:02:03", to_string(t, format::EXTENDED));
}

TEST(iso8601_test, test_datetime_to_string) {
    datetime dt(2000, 1, 2, 3, 4, 5, 6);
    EXPECT_EQ("20000102T030405.000006", to_string(dt));
}

TEST(iso8601_test, test_datetime_to_string_extended) {
    datetime dt(2000, 1, 2, 3, 4, 5, 6);
    EXPECT_EQ("2000-01-02T03:04:05.000006", to_string(dt, format::EXTENDED));
}

} // namepsace iso8601
} // namespace brfc
