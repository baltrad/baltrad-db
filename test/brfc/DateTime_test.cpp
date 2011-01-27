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

#include <ctime>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <string>
#include <brfc/TimeDelta.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

TEST(DateTime_test, test_eq) {
    DateTime dt1(2000, 2, 1);
    EXPECT_TRUE(dt1 == dt1);
    EXPECT_FALSE(dt1 != dt1);

    DateTime dt2(2000, 2, 1, 0, 0, 0, 1);
    EXPECT_FALSE(dt1 == dt2);
    EXPECT_TRUE(dt1 != dt2);
}

TEST(DateTime_test, test_lt) {
    DateTime dt1(2000, 2, 1);
    DateTime dt2(2000, 2, 1, 0, 0, 0, 1);
    DateTime dt3(2000, 2, 2);

    EXPECT_TRUE(dt1 < dt2);
    EXPECT_TRUE(dt1 < dt3);
    EXPECT_TRUE(dt2 < dt3);

    EXPECT_FALSE(dt1 < dt1);
    EXPECT_FALSE(dt2 < dt1);
    EXPECT_FALSE(dt3 < dt1);
    EXPECT_FALSE(dt3 < dt2);
}

TEST(Datetime_test, test_add) {
    DateTime dt1(2000, 2, 1, 12);
    
    dt1 += TimeDelta().add_days(2).add_hours(13);
    
    EXPECT_EQ(DateTime(2000, 2, 4, 1), dt1);
}

TEST(DateTime_test, test_to_string) {
    DateTime dt1(2010, 10, 27, 12, 13, 14, 567);
    EXPECT_EQ("2010-10-27 12:13:14.567", dt1.to_string("yyyy-MM-dd hh:mm:ss.zzz"));
}

TEST(DateTime_test, test_now) {
    // not a foolproof way to test, expect some failures now and then
    std::tm t;
    time_t time = std::time(0);
    localtime_r(&time, &t);
    DateTime dt = DateTime::now();
    EXPECT_EQ(1900 + t.tm_year, dt.date().year());
    EXPECT_EQ(t.tm_mon + 1, dt.date().month());
    EXPECT_EQ(t.tm_mday, dt.date().day());
    EXPECT_EQ(t.tm_hour, dt.time().hour());
}

TEST(DateTime_test, test_utc_now) {
    std::tm t;
    time_t time = std::time(0);
    gmtime_r(&time, &t);
    DateTime dt = DateTime::utc_now();
    EXPECT_EQ(1900 + t.tm_year, dt.date().year());
    EXPECT_EQ(t.tm_mon + 1, dt.date().month());
    EXPECT_EQ(t.tm_mday, dt.date().day());
    EXPECT_EQ(t.tm_hour, dt.time().hour());
}

TEST(DateTime_test, test_to_tm) {
    DateTime dt1(1899, 10, 27, 12, 13, 14, 567);
    EXPECT_THROW(dt1.to_tm(), value_error);

    DateTime dt2(2010, 10, 27, 12, 13, 14, 567);
    struct tm t = dt2.to_tm();
    EXPECT_EQ(t.tm_year, 110);
    EXPECT_EQ(t.tm_mon, 9);
    EXPECT_EQ(t.tm_mday, 27);
    EXPECT_EQ(t.tm_hour, 12);
    EXPECT_EQ(t.tm_min, 13);
    EXPECT_EQ(t.tm_sec, 14);
}

} // namespace brfc
