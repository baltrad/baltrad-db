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
#include <brfc/exceptions.hpp>

#include "common.hpp"

using namespace brfc;

TEST(Variant_test, null) {
    Variant v;
    EXPECT_TRUE(v.is_null());
    EXPECT_EQ(v.type(), Variant::NONE);
    EXPECT_FALSE(v.is_string());
    EXPECT_THROW(v.int64_(), value_error);
}

TEST(Variant_test, int64_) {
    Variant v(10); // accepts integers
    EXPECT_FALSE(v.is_null());
    EXPECT_EQ(v.int64_(), 10);
    EXPECT_EQ(v.type(), Variant::INT64);
    EXPECT_EQ(v.type(), Variant::LONGLONG); // XXX: deprecated
    EXPECT_THROW(v.string(), value_error);
}

TEST(Variant_test, copy_ctor) {
    Variant v1(10);
    Variant v2 = v1;
    EXPECT_FALSE(v2.is_null());
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(Variant_test, copy_assign) {
    Variant v1(10);
    Variant v2;
    v2 = v1;
    EXPECT_FALSE(v2.is_null());
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(Variant_test, string) {
    String val = String::from_utf8("öäõxyz");
    Variant v(val);
    EXPECT_EQ(v.type(), Variant::STRING);
    EXPECT_EQ(v.string(), val);
    EXPECT_FALSE(v.is_null());
    EXPECT_THROW(v.int64_(), value_error);
}

TEST(Variant_test, from_string_literal) {
    Variant v("asd");
    EXPECT_EQ(v.type(), Variant::STRING);
}

TEST(Variant_test, equality_comparison) {
    Variant v1(1);
    Variant v2;
    Variant v3(1);
    Variant v4("asdasd");
    EXPECT_EQ(v1, v1);
    EXPECT_NE(v1, v2);
    EXPECT_EQ(v1, v3);
    EXPECT_NE(v1, v4);
    EXPECT_EQ(v2, v2);
    EXPECT_NE(v2, v3);
    EXPECT_NE(v2, v4);
    EXPECT_EQ(v3, v3);
    EXPECT_NE(v3, v4);
    EXPECT_EQ(v4, v4);
}

TEST(Variant_test, to_string) {
    EXPECT_EQ(Variant().to_string(), "");
    EXPECT_EQ(Variant("foo").to_string(), "foo");
    EXPECT_EQ(Variant(10).to_string(), "10");
    EXPECT_EQ(Variant(1.2).to_string(), "1.2");
    EXPECT_EQ(Variant(true).to_string(), "True");
    EXPECT_EQ(Variant(false).to_string(), "False");
    EXPECT_EQ(Variant(Date(2000, 11, 12)).to_string(), "20001112");
    EXPECT_EQ(Variant(Time(12, 34, 56)).to_string(), "123456");
}

TEST(Variant_test, to_int64) {
    EXPECT_EQ(0, Variant().to_int64());
    EXPECT_EQ(10, Variant("10").to_int64());
    EXPECT_THROW(Variant("foo").to_int64(), value_error);
    EXPECT_EQ(10, Variant(10).to_int64());
    EXPECT_EQ(1, Variant(1.2).to_int64());
    EXPECT_EQ(1, Variant(true).to_int64());
    EXPECT_EQ(0, Variant(false).to_int64());
    EXPECT_THROW(Variant(Date(2000, 11, 12)).to_int64(), value_error);
    EXPECT_THROW(Variant(Time(12, 34, 56)).to_int64(), value_error);
}

TEST(Variant_test, to_double) {
    EXPECT_EQ(0.0, Variant().to_double());
    EXPECT_EQ(10.0, Variant("10").to_double());
    EXPECT_THROW(Variant("foo").to_double(), value_error);
    EXPECT_EQ(10.0, Variant(10).to_double());
    EXPECT_EQ(1.2, Variant(1.2).to_double());
    EXPECT_EQ(1.0, Variant(true).to_double());
    EXPECT_EQ(0.0, Variant(false).to_double());
    EXPECT_THROW(Variant(Date(2000, 11, 12)).to_double(), value_error);
    EXPECT_THROW(Variant(Time(12, 34, 56)).to_double(), value_error);
}

TEST(Variant_test, to_bool) {
    EXPECT_EQ(false, Variant().to_bool());
    EXPECT_EQ(true, Variant("foo").to_bool());
    EXPECT_EQ(false, Variant("").to_bool());
    EXPECT_EQ(true, Variant(10).to_bool());
    EXPECT_EQ(true, Variant(1.2).to_bool());
    EXPECT_EQ(true, Variant(true).to_bool());
    EXPECT_EQ(false, Variant(false).to_bool());
    EXPECT_EQ(true, Variant(Date(2000, 11, 12)).to_bool());
    EXPECT_EQ(true, Variant(Time(12, 34, 56)).to_bool());
}

TEST(Variant_test, to_time) {
    EXPECT_THROW(Variant().to_time(), value_error);
    EXPECT_THROW(Variant("foo").to_time(), value_error);
    EXPECT_EQ(Time(12, 34, 56), Variant("12:34:56").to_time());
    EXPECT_THROW(Variant(10).to_time(), value_error);
    EXPECT_THROW(Variant(1.2).to_time(), value_error);
    EXPECT_THROW(Variant(true).to_time(), value_error);
    EXPECT_THROW(Variant(false).to_time(), value_error);
    EXPECT_THROW(Variant(Date(2000, 11, 12)).to_time(), value_error);
    EXPECT_EQ(Time(12, 34, 56), Variant(Time(12, 34, 56)).to_time());
}

TEST(Variant_test, to_date) {
    EXPECT_THROW(Variant().to_date(), value_error);
    EXPECT_THROW(Variant("foo").to_date(), value_error);
    EXPECT_EQ(Date(2000, 11, 12), Variant("2000-11-12").to_date());
    EXPECT_THROW(Variant(10).to_date(), value_error);
    EXPECT_THROW(Variant(1.2).to_date(), value_error);
    EXPECT_THROW(Variant(true).to_date(), value_error);
    EXPECT_THROW(Variant(false).to_date(), value_error);
    EXPECT_EQ(Date(2000, 11, 12), Variant(Date(2000, 11, 12)).to_date());
    EXPECT_THROW(Variant(Time(12, 34, 56)).to_date(), value_error);
}
