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
#include <brfc/DateTime.hpp>
#include <brfc/String.hpp>
#include <brfc/TimeDelta.hpp>

#include "common.hpp"

namespace brfc {

TEST(DateTime_test, test_eq) {
    DateTime dt1(2000, 2, 1);
    EXPECT_TRUE(dt1 == dt1);
    EXPECT_FALSE(dt1 != dt1);

    DateTime dt2(2000, 2, 1, 0, 0, 0, 1);
    EXPECT_FALSE(dt1 == dt2);
    EXPECT_TRUE(dt1 != dt2);
}

TEST(Datetime_test, test_add) {
    DateTime dt1(2000, 2, 1, 12);
    
    dt1 += TimeDelta().add_days(2).add_hours(13);
    
    EXPECT_EQ(DateTime(2000, 2, 4, 1), dt1);
}

} // namespace brfc
