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

#include <memory>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
#include <brfc/util/Url.hpp>

namespace brfc {
namespace sql {

TEST(sql_DefaultConnectionCreator_test, test_create_valid_url) {
    Url url("postgresql://user:password@unknown-host/dbname");
    DefaultConnectionCreator c(url);
    std::auto_ptr<Connection> p;    
    EXPECT_THROW(p.reset(c.create()), db_error); // url is valid, no db though
}

TEST(sql_DefaultConnectionCreator_test, test_create_invalid_url) {
    Url url("bla://user:password@localhost/dbname");
    DefaultConnectionCreator c(url);
    std::auto_ptr<Connection> p;
    EXPECT_THROW(p.reset(c.create()), std::invalid_argument);
}

} // namespace sql
} // namespace brfc
