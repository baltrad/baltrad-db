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

#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionCreator.hpp>
#include <brfc/sql/PoolReturner.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_BasicConnectionPool_test : public ::testing::Test {
  public:
    sql_BasicConnectionPool_test()
        : creator()
        , pool(&creator) {

    }

    virtual void SetUp() {
    }
    
    MockConnectionCreator creator;
    BasicConnectionPool pool;
};

TEST_F(sql_BasicConnectionPool_test, test_get) {
    MockConnection* conn = new MockConnection();

    EXPECT_CALL(creator, do_create())
        .WillOnce(Return(conn));
    EXPECT_CALL(*conn, do_is_open())
        .WillOnce(Return(false));
    EXPECT_CALL(*conn, do_open());
    
    auto_ptr<Connection> c(pool.get());
    ConnectionProxy* cp = dynamic_cast<ConnectionProxy*>(c.get());

    ASSERT_TRUE(cp);
    Connection* proxied = &cp->proxied();

    c.reset(); // put back to pool
    
    // same connection
    c.reset(pool.get());
    cp = dynamic_cast<ConnectionProxy*>(c.get());
    ASSERT_TRUE(cp);
    EXPECT_EQ(proxied, &cp->proxied());
}

TEST_F(sql_BasicConnectionPool_test, test_dtor) {
    PoolReturner returner;
    {
        BasicConnectionPool p(&creator, &returner);
        EXPECT_EQ(&p, returner.pool());
    }
    
    EXPECT_FALSE(returner.pool());
}

} // namespace sql
} // namespace brfc
