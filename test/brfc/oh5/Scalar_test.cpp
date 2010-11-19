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
#include <brfc/Time.hpp>

#include <brfc/oh5/Scalar.hpp>

#include <brfc/test_common.hpp>

namespace brfc {
namespace oh5 {

TEST(Scalar_test, int64_) {
    Scalar v(10); // accepts integers
    EXPECT_EQ(v.int64_(), 10);
    EXPECT_EQ(v.type(), Scalar::INT64);
    EXPECT_THROW(v.string(), value_error);
    EXPECT_THROW(v.double_(), value_error);
}

TEST(Scalar_test, test_ctor_from_bool) {
    Scalar v(true);
    EXPECT_EQ(v.type(), Scalar::STRING);
    EXPECT_EQ("True", v.string());
    
    v = Scalar(false);
    EXPECT_EQ("False", v.string());
}

TEST(Scalar_test, test_ctor_from_date) {
    Scalar v(Date(2000, 9, 15));
    EXPECT_EQ(v.type(), Scalar::STRING); 
    EXPECT_EQ("20000915", v.string());
}

TEST(Scalar_test, test_ctor_from_time) {
    Scalar v(Time(12, 13, 14, 15));
    EXPECT_EQ(v.type(), Scalar::STRING);
    EXPECT_EQ("121314", v.string());
}

TEST(Scalar_test, copy_ctor) {
    Scalar v1(10);
    Scalar v2 = v1;
    EXPECT_EQ(v2.type(), Scalar::INT64);
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(Scalar_test, copy_assign) {
    Scalar v1(10);
    Scalar v2(1);
    v2 = v1;
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(Scalar_test, string) {
    String val = String::from_utf8("öäõxyz");
    Scalar v(val);
    EXPECT_EQ(v.type(), Scalar::STRING);
    EXPECT_EQ(v.string(), val);
    EXPECT_THROW(v.int64_(), value_error);
}

TEST(Scalar_test, from_string_literal) {
    Scalar v("asd");
    EXPECT_EQ(v.type(), Scalar::STRING);
}

TEST(Scalar_test, equality_comparison) {
    Scalar v1(1);
    Scalar v2(1.1);
    Scalar v3(1);
    Scalar v4("asdasd");
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

TEST(Scalar_test, to_string) {
    EXPECT_EQ(Scalar("foo").to_string(), "foo");
    EXPECT_EQ(Scalar(10).to_string(), "10");
    EXPECT_EQ(Scalar(1.2).to_string(), "1.2");
}

TEST(Scalar_test, to_int64) {
    EXPECT_EQ(10, Scalar("10").to_int64());
    EXPECT_THROW(Scalar("foo").to_int64(), value_error);
    EXPECT_EQ(10, Scalar(10).to_int64());
    EXPECT_EQ(1, Scalar(1.2).to_int64());
}

TEST(Scalar_test, to_double) {
    EXPECT_EQ(10.0, Scalar("10").to_double());
    EXPECT_THROW(Scalar("foo").to_double(), value_error);
    EXPECT_EQ(10.0, Scalar(10).to_double());
    EXPECT_EQ(1.2, Scalar(1.2).to_double());
}

TEST(Scalar_test, to_bool) {
    EXPECT_THROW(Scalar("foo").to_bool(), value_error);
    EXPECT_THROW(Scalar("").to_bool(), value_error);
    EXPECT_EQ(true, Scalar("True").to_bool());
    EXPECT_EQ(false, Scalar("False").to_bool());
    EXPECT_EQ(true, Scalar(10).to_bool());
    EXPECT_EQ(true, Scalar(1.2).to_bool());
}

TEST(Scalar_test, to_time) {
    EXPECT_THROW(Scalar("foo").to_time(), value_error);
    EXPECT_EQ(Time(12, 34, 56), Scalar("123456").to_time());
    EXPECT_THROW(Scalar(10).to_time(), value_error);
    EXPECT_THROW(Scalar(1.2).to_time(), value_error);
}

TEST(Scalar_test, to_date) {
    EXPECT_THROW(Scalar("foo").to_date(), value_error);
    EXPECT_EQ(Date(2000, 11, 12), Scalar("20001112").to_date());
    EXPECT_THROW(Scalar(10).to_date(), value_error);
    EXPECT_THROW(Scalar(1.2).to_date(), value_error);
}

} // namespace oh5
} // namespace brfc
