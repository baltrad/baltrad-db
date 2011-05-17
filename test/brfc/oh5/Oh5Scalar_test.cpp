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
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5Scalar.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

TEST(oh5_Oh5Scalar_test, int64_) {
    Oh5Scalar v(10); // accepts integers
    EXPECT_EQ(v.int64_(), 10);
    EXPECT_EQ(v.type(), Oh5Scalar::INT64);
    EXPECT_THROW(v.string(), value_error);
    EXPECT_THROW(v.double_(), value_error);
}

TEST(oh5_Oh5Scalar_test, test_ctor_from_bool) {
    Oh5Scalar v(true);
    EXPECT_EQ(v.type(), Oh5Scalar::STRING);
    EXPECT_EQ("True", v.string());
    
    v = Oh5Scalar(false);
    EXPECT_EQ("False", v.string());
}

TEST(oh5_Oh5Scalar_test, test_ctor_from_date) {
    Oh5Scalar v(Date(2000, 9, 15));
    EXPECT_EQ(v.type(), Oh5Scalar::STRING); 
    EXPECT_EQ("20000915", v.string());
}

TEST(oh5_Oh5Scalar_test, test_ctor_from_time) {
    Oh5Scalar v(Time(12, 13, 14, 15));
    EXPECT_EQ(v.type(), Oh5Scalar::STRING);
    EXPECT_EQ("121314", v.string());
}

TEST(oh5_Oh5Scalar_test, copy_ctor) {
    Oh5Scalar v1(10);
    Oh5Scalar v2 = v1;
    EXPECT_EQ(v2.type(), Oh5Scalar::INT64);
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(oh5_Oh5Scalar_test, copy_assign) {
    Oh5Scalar v1(10);
    Oh5Scalar v2(1);
    v2 = v1;
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.int64_(), v1.int64_());
}

TEST(oh5_Oh5Scalar_test, string) {
    std::string val("öäõxyz");
    Oh5Scalar v(val);
    EXPECT_EQ(v.type(), Oh5Scalar::STRING);
    EXPECT_EQ(v.string(), val);
    EXPECT_THROW(v.int64_(), value_error);
}

TEST(oh5_Oh5Scalar_test, from_string_literal) {
    Oh5Scalar v("asd");
    EXPECT_EQ(v.type(), Oh5Scalar::STRING);
}

TEST(oh5_Oh5Scalar_test, equality_comparison) {
    Oh5Scalar v1(1);
    Oh5Scalar v2(1.1);
    Oh5Scalar v3(1);
    Oh5Scalar v4("asdasd");
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

TEST(oh5_Oh5Scalar_test, to_string) {
    EXPECT_EQ(Oh5Scalar("foo").to_string(), "foo");
    EXPECT_EQ(Oh5Scalar(10).to_string(), "10");
    EXPECT_EQ(Oh5Scalar(1.2).to_string(), "1.2");
}

TEST(oh5_Oh5Scalar_test, to_int64) {
    EXPECT_EQ(10, Oh5Scalar("10").to_int64());
    EXPECT_THROW(Oh5Scalar("foo").to_int64(), value_error);
    EXPECT_EQ(10, Oh5Scalar(10).to_int64());
    EXPECT_EQ(1, Oh5Scalar(1.2).to_int64());
}

TEST(oh5_Oh5Scalar_test, to_double) {
    EXPECT_EQ(10.0, Oh5Scalar("10").to_double());
    EXPECT_THROW(Oh5Scalar("foo").to_double(), value_error);
    EXPECT_EQ(10.0, Oh5Scalar(10).to_double());
    EXPECT_EQ(1.2, Oh5Scalar(1.2).to_double());
}

TEST(oh5_Oh5Scalar_test, to_bool) {
    EXPECT_THROW(Oh5Scalar("foo").to_bool(), value_error);
    EXPECT_THROW(Oh5Scalar("").to_bool(), value_error);
    EXPECT_EQ(true, Oh5Scalar("True").to_bool());
    EXPECT_EQ(false, Oh5Scalar("False").to_bool());
    EXPECT_EQ(true, Oh5Scalar(10).to_bool());
    EXPECT_EQ(true, Oh5Scalar(1.2).to_bool());
}

TEST(oh5_Oh5Scalar_test, to_time) {
    EXPECT_THROW(Oh5Scalar("foo").to_time(), value_error);
    EXPECT_EQ(Time(12, 34, 56), Oh5Scalar("123456").to_time());
    EXPECT_THROW(Oh5Scalar(10).to_time(), value_error);
    EXPECT_THROW(Oh5Scalar(1.2).to_time(), value_error);
}

TEST(oh5_Oh5Scalar_test, to_date) {
    EXPECT_THROW(Oh5Scalar("foo").to_date(), value_error);
    EXPECT_EQ(Date(2000, 11, 12), Oh5Scalar("20001112").to_date());
    EXPECT_THROW(Oh5Scalar(10).to_date(), value_error);
    EXPECT_THROW(Oh5Scalar(1.2).to_date(), value_error);
}

} // namespace brfc