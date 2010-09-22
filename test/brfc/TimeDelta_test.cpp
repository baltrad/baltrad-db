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

#include <brfc/TimeDelta.hpp>

namespace brfc {

TEST(TimeDelta_test, test_add) {
    TimeDelta td;
    EXPECT_EQ(0, td.msecs());
    EXPECT_EQ(0, td.days());

    td.add_minutes(1);
    EXPECT_EQ(60000, td.msecs());
    EXPECT_EQ(0, td.days());
    td.add_minutes(-2);
    EXPECT_EQ(86340000, td.msecs());
    EXPECT_EQ(-1, td.days());
    td.add_hours(24);
    EXPECT_EQ(86340000, td.msecs());
    EXPECT_EQ(0, td.days());
    td.add_minutes(1);
    td.add_hours(48);
    EXPECT_EQ(0, td.msecs());
    EXPECT_EQ(2, td.days());
}

} // namespace brfc
