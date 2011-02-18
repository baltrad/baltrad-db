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

#include <brfc/sql/MockConnectionPool.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/PoolReturner.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

TEST(sql_PoolReturner_test, test_destroy) {
    MockConnectionPool p;
    MockConnection c;
    PoolReturner r(&p);

    EXPECT_CALL(p, do_put(&c));

    r.destroy(&c);
}

TEST(sql_PoolReturner_test, test_destroy_no_pool) {
    MockConnection* c = new MockConnection();
    PoolReturner r;
    
    ON_CALL(*c, do_is_open())
        .WillByDefault(Return(true)); // for leak detection
    
    r.destroy(c);
}

} // namespace sql
} // namespace brfc