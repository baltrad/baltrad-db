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

#include <brfc/Url.hpp>

namespace brfc {

TEST(Url_test, test_complete_url) {
    Url url("http://user:password@example.com:1234/path/to/resource");
    EXPECT_EQ("http", url.scheme());
    EXPECT_EQ("user", url.user_name());
    EXPECT_EQ("password", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(1234, url.port());
    EXPECT_EQ("/path/to/resource", url.path());
}

} // namespace brfc
