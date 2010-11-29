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

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>

namespace brfc {
namespace sql {

TEST(sql_DefaultConnectionCreator_test, test_create_valid_url) {
    DefaultConnectionCreator c("postgresql://user:password@localhost/dbname");
    auto_ptr<Connection> p;    
    EXPECT_NO_THROW(p.reset(c.create()));
}

TEST(sql_DefaultConnectionCreator_test, test_create_invalid_url) {
    DefaultConnectionCreator c("bla://user:password@localhost/dbname");
    auto_ptr<Connection> p;
    EXPECT_THROW(p.reset(c.create()), value_error);
}

} // namespace sql
} // namespace brfc