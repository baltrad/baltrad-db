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

#include <ctime>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <string>
#include <brfc/Time.hpp>
#include <brfc/TimeDelta.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

TEST(Time_test, test_construct_invalid_time) {
    EXPECT_THROW(Time(24, 0), std::invalid_argument);
    EXPECT_THROW(Time(0, 60), std::invalid_argument);
    EXPECT_THROW(Time(0, 0, 60), std::invalid_argument);
    EXPECT_THROW(Time(0, 0, 0, 1000), std::invalid_argument);
}

TEST(Time_test, test_accessors) {
    Time t(12, 5, 6, 7);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(7, t.msec());
}

TEST(Time_test, test_set_hour) {
    Time t(12, 5, 6, 7);
    t.hour(15);
    EXPECT_EQ(15, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(7, t.msec());

    t.hour(1);
    EXPECT_EQ(1, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(7, t.msec());

    EXPECT_THROW(t.hour(-1), std::invalid_argument);
    EXPECT_THROW(t.hour(24), std::invalid_argument);
}

TEST(Time_test, test_set_minute) {
    Time t(12, 5, 6, 7);
    t.minute(58);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(58, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(7, t.msec());

    t.minute(1);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(1, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(7, t.msec());

    EXPECT_THROW(t.minute(-1), std::invalid_argument);
    EXPECT_THROW(t.minute(60), std::invalid_argument);
}

TEST(Time_test, test_set_second) {
    Time t(12, 5, 6, 7);
    t.second(58);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(58, t.second());
    EXPECT_EQ(7, t.msec());

    t.second(1);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(1, t.second());
    EXPECT_EQ(7, t.msec());

    EXPECT_THROW(t.second(-1), std::invalid_argument);
    EXPECT_THROW(t.second(60), std::invalid_argument);
}

TEST(Time_test, test_set_msec) {
    Time t(12, 5, 6, 7);
    t.msec(586);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(586, t.msec());

    t.msec(1);
    EXPECT_EQ(12, t.hour());
    EXPECT_EQ(5, t.minute());
    EXPECT_EQ(6, t.second());
    EXPECT_EQ(1, t.msec());

    EXPECT_THROW(t.msec(-1), std::invalid_argument);
    EXPECT_THROW(t.msec(1000), std::invalid_argument);
}

TEST(Time_test, test_eq) {
    Time t1(12, 0);
    EXPECT_TRUE(t1 == t1);
    EXPECT_FALSE(t1 != t1);

    Time t2(12, 0, 0, 1);
    EXPECT_FALSE(t1 == t2);
    EXPECT_TRUE(t1 != t2);
}

TEST(Time_test, test_lt) {
    Time t1(12, 0);
    Time t2(12, 0, 0, 1);

    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t1 < t1);
    EXPECT_FALSE(t2 < t1);
}

TEST(Time_test, test_add_hours) {
    Time t(12, 0);
    EXPECT_EQ(Time(13, 0), t + TimeDelta().add_hours(1));
    EXPECT_EQ(Time(1, 0), t + TimeDelta().add_hours(13));
    EXPECT_EQ(t, t + TimeDelta().add_hours(24));
    EXPECT_EQ(t, t + TimeDelta().add_hours(-24));
    EXPECT_EQ(Time(13, 0), t + TimeDelta().add_hours(49));
    EXPECT_EQ(Time(11, 0), t + TimeDelta().add_hours(-49));
}

TEST(Time_test, test_add_minutes) {
    Time t(12, 0);
    EXPECT_EQ(Time(12, 35), t + TimeDelta().add_minutes(35));
    EXPECT_EQ(Time(11, 35), t + TimeDelta().add_minutes(-25));
    EXPECT_EQ(t, t + TimeDelta().add_minutes(1440));
    EXPECT_EQ(t, t + TimeDelta().add_minutes(-1440));
    EXPECT_EQ(Time(14, 0), t + TimeDelta().add_minutes(3000));
    EXPECT_EQ(Time(10, 0), t + TimeDelta().add_minutes(-3000));
}

TEST(Time_test, test_add_seconds) {
    Time t(12, 0);
    EXPECT_EQ(Time(12, 1, 30), t + TimeDelta().add_seconds(90));
    EXPECT_EQ(Time(11, 58, 30), t + TimeDelta().add_seconds(-90));
    EXPECT_EQ(t, t + TimeDelta().add_seconds(86400));
    EXPECT_EQ(t, t + TimeDelta().add_seconds(-86400));
    EXPECT_EQ(Time(12, 1, 59), t + TimeDelta().add_seconds(172919));
    EXPECT_EQ(Time(11, 58, 1), t + TimeDelta().add_seconds(-172919));
}

TEST(Time_test, test_add_msecs) {
    Time t(12, 0);
    EXPECT_EQ(Time(12, 1, 0, 1), t + TimeDelta().add_msecs(60001));
    EXPECT_EQ(Time(11, 59, 59, 999), t + TimeDelta().add_msecs(-1));
    EXPECT_EQ(t, t + TimeDelta().add_msecs(86400000));
    EXPECT_EQ(t, t + TimeDelta().add_msecs(-86400000));
    EXPECT_EQ(Time(12, 1, 59, 1), t + TimeDelta().add_msecs(172919001));
    EXPECT_EQ(Time(11, 58, 0, 999), t + TimeDelta().add_msecs(-172919001));
}

TEST(Time_test, test_from_iso_string) {
    EXPECT_EQ(Time(12, 13, 14, 15), Time::from_iso_string("12:13:14.015"));
    EXPECT_EQ(Time(12, 13, 14, 15), Time::from_iso_string("121314.015"));
    EXPECT_EQ(Time(12, 13, 14, 0), Time::from_iso_string("12:13:14"));
    EXPECT_THROW(Time::from_iso_string("121314015"), std::invalid_argument);
    EXPECT_THROW(Time::from_iso_string("12-13-14"), std::invalid_argument);
}

TEST(Time_test, test_to_iso_string) {
    EXPECT_EQ("121314.015000", Time(12, 13, 14, 15).to_iso_string());
    EXPECT_EQ("121314", Time(12, 13, 14).to_iso_string());
}

TEST(Time_test, test_to_iso_string_extended) {
    EXPECT_EQ("12:13:14.015000", Time(12, 13, 14, 15).to_iso_string(true));
    EXPECT_EQ("12:13:14", Time(12, 13, 14).to_iso_string(true));
}

} // namespace brfc
