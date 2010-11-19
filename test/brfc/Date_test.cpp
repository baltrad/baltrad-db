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
#include <brfc/Date.hpp>
#include <brfc/String.hpp>
#include <brfc/TimeDelta.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

TEST(Date_test, test_is_leap_year) {
    EXPECT_TRUE(Date::is_leap_year(1600));
    EXPECT_TRUE(Date::is_leap_year(2000));
    EXPECT_TRUE(Date::is_leap_year(2004));
    
    EXPECT_FALSE(Date::is_leap_year(1700));
    EXPECT_FALSE(Date::is_leap_year(1800));
    EXPECT_FALSE(Date::is_leap_year(1900)); 
    EXPECT_FALSE(Date::is_leap_year(1901));
}

TEST(Date_test, test_jdn_from_date) {
    int year = -4713, month = 11, day = 24;
    EXPECT_EQ(0, Date::jdn_from_date(year, month, day));
}

TEST(Date_test, test_date_from_jdn) {
    int year, month, day;
    Date::date_from_jdn(0, year, month, day);
    EXPECT_EQ(-4713, year);
    EXPECT_EQ(11, month);
    EXPECT_EQ(24, day);
    
    Date::date_from_jdn(2451545, year, month, day);
    EXPECT_EQ(2000, year);
    EXPECT_EQ(1, month);
    EXPECT_EQ(1, day);
}

TEST(Date_test, test_days_in_month) {
    EXPECT_EQ(29, Date::days_in_month(2000, 2));
    EXPECT_EQ(28, Date::days_in_month(2001, 2));
    EXPECT_EQ(31, Date::days_in_month(2000, 1));
    EXPECT_THROW(Date::days_in_month(2000, 13), value_error);
}

TEST(Date_test, test_construct_invalid_date) {
    EXPECT_THROW(Date(1, 13, 1), value_error);
    EXPECT_THROW(Date(1, 12, 32), value_error);
    EXPECT_THROW(Date(0, 12, 1), value_error);
}

TEST(Date_test, test_construct_february_29) {
    EXPECT_THROW(Date(2001, 2, 29), value_error);
    EXPECT_NO_THROW(Date(2001, 2, 28));
    EXPECT_NO_THROW(Date(2000, 2, 29));
}

TEST(Date_test, test_accessors) {
    Date d(2000, 2, 15);
    EXPECT_EQ(2000, d.year());
    EXPECT_EQ(2, d.month());
    EXPECT_EQ(15, d.day());
}

TEST(Date_test, test_set_year) {
    Date d(2000, 2, 15);
    d.year(500);
    EXPECT_EQ(500, d.year());
    EXPECT_EQ(2, d.month());
    EXPECT_EQ(15, d.day());

    d.year(2500);
    EXPECT_EQ(2500, d.year());
    EXPECT_EQ(2, d.month());
    EXPECT_EQ(15, d.day());
    
    EXPECT_THROW(Date(2000, 2, 29).year(2001), value_error);
}

TEST(Date_test, test_set_month) {
    Date d(2000, 2, 15);
    d.month(12);
    EXPECT_EQ(2000, d.year());
    EXPECT_EQ(12, d.month());
    EXPECT_EQ(15, d.day());

    d.month(1);
    EXPECT_EQ(2000, d.year());
    EXPECT_EQ(1, d.month());
    EXPECT_EQ(15, d.day());
    
    EXPECT_THROW(d.month(0), value_error);
    EXPECT_THROW(d.month(13), value_error);
    EXPECT_THROW(Date(2000, 1, 31).month(2), value_error);
}

TEST(Date_test, test_set_day) {
    Date d(2000, 2, 15);
    d.day(29);
    EXPECT_EQ(2000, d.year());
    EXPECT_EQ(2, d.month());
    EXPECT_EQ(29, d.day());

    d.day(1);
    EXPECT_EQ(2000, d.year());
    EXPECT_EQ(2, d.month());
    EXPECT_EQ(1, d.day());
    
    EXPECT_THROW(d.day(0), value_error);
    EXPECT_THROW(Date(2000, 2, 29).day(30), value_error);
}

TEST(Date_test, test_eq) {
    Date d1(2000, 2, 1);
    EXPECT_TRUE(d1 == d1);
    EXPECT_FALSE(d1 != d1);
    
    Date d2(2000, 2, 2);
    EXPECT_FALSE(d1 == d2);
    EXPECT_TRUE(d1 != d2);
}

TEST(Date_test, test_add_days) {
    Date d1(2000, 2, 29);
    EXPECT_EQ(Date(2000, 3, 1), d1 + TimeDelta().add_days(1));
    EXPECT_EQ(Date(2000, 2, 28), d1 + TimeDelta().add_days(-1));
    EXPECT_EQ(Date(2004, 2, 29), d1 + TimeDelta().add_days(365 * 4 + 1));
    EXPECT_EQ(Date(1999, 12, 31), d1 + TimeDelta().add_days(-60));
}

TEST(Date_test, test_to_string) {
    EXPECT_EQ("20000102", Date(2000, 1, 2).to_string("yyyyMMdd"));
    EXPECT_EQ("2000-01-02", Date(2000, 1, 2).to_string("yyyy-MM-dd"));
}

TEST(Date_test, test_from_string) {
    EXPECT_EQ(Date(2000, 1, 2),
              Date::from_string("20000102", "yyyyMMdd"));
    EXPECT_EQ(Date(2000, 1, 2),
              Date::from_string("2000-01-02", "yyyy-MM-dd"));
    EXPECT_THROW(Date::from_string("200122", "yyyyMMdd"), value_error);
}

TEST(Date_test, test_from_iso_string) {
    EXPECT_EQ(Date(2000, 1, 2), Date::from_iso_string("20000102"));
    EXPECT_THROW(Date::from_iso_string("2000:01:02"), value_error);
}

TEST(Date_test, test_from_extended_iso_string) {
    EXPECT_EQ(Date(2000, 1, 2), Date::from_extended_iso_string("2000-01-02"));
    EXPECT_THROW(Date::from_extended_iso_string("2000:01:02"), value_error);
    EXPECT_THROW(Date::from_extended_iso_string("20000102"), value_error);
}

} // namespace brfc
