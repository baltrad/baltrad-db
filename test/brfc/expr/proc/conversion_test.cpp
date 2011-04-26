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
#include <brfc/test_common.hpp>
#include <brfc/expr/listcons.hpp>
#include <brfc/expr/proc/conversion.hpp>

namespace brfc {
namespace expr {
namespace proc {

/* date */

TEST(expr_proc_conversion_date_test, test_Expression_dispatch) {
    Expression e(Date(2011, 12, 13));
    EXPECT_EQ(e, date()(listcons().append(e).get()));
}

TEST(expr_proc_conversion_date_test, test_string) {
    Expression e(Date(2011, 12, 13));
    EXPECT_EQ(e, date()(std::string("2011-12-13")));
}

TEST(expr_proc_conversion_date_test, test_string_invalid) {
    EXPECT_THROW(date()(std::string("foo")), value_error);
}

TEST(expr_proc_conversion_date_test, test_date) {
    Expression e(Date(2011, 12, 13));
    EXPECT_EQ(e, date()(Date(2011, 12, 13)));
}

TEST(expr_proc_conversion_date_test, test_datetime) {
    Expression e(Date(2011, 12, 13));
    EXPECT_EQ(e, date()(DateTime(2011, 12, 13, 14, 15, 16)));
}

/* time */

TEST(expr_proc_conversion_time_test, test_Expression_dispatch) {
    Expression e(Time(14, 15, 16));
    EXPECT_EQ(e, time()(listcons().append(e).get()));
}

TEST(expr_proc_conversion_time_test, test_string) {
    Expression e(Time(14, 15, 16));
    EXPECT_EQ(e, time()(std::string("14:15:16")));
}

TEST(expr_proc_conversion_time_test, test_string_invalid) {
    EXPECT_THROW(time()(std::string("foo")), value_error);
}

TEST(expr_proc_conversion_time_test, test_time) {
    Expression e(Time(14, 15, 16));
    EXPECT_EQ(e, time()(Time(14, 15, 16)));
}

TEST(expr_proc_conversion_time_test, test_datetime) {
    Expression e(Time(14, 15, 16));
    EXPECT_EQ(e, time()(DateTime(2011, 12, 13, 14, 15, 16)));
}

/* datetime */

TEST(expr_proc_conversion_datetime_test, test_Expression_dispatch) {
    Expression e(DateTime(2011, 12, 13, 14, 15, 16));
    EXPECT_EQ(e, datetime()(listcons().append(e).get()));
}

TEST(expr_proc_conversion_datetime_test, test_string) {
    Expression e(DateTime(2011, 12, 13, 14, 15, 16));
    EXPECT_EQ(e, datetime()(std::string("2011-12-13T14:15:16")));
}

TEST(expr_proc_conversion_datetime_test, test_string_invalid) {
    EXPECT_THROW(datetime()(std::string("foo")), value_error);
}


TEST(expr_proc_conversion_datetime_test, test_datetime) {
    Expression e(DateTime(2011, 12, 13, 14, 15, 16));
    EXPECT_EQ(e, datetime()(DateTime(2011, 12, 13, 14, 15, 16)));
}

} // namespace proc
} // namespace expr
} // namespace brfc
