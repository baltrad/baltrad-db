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

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_BasicConnectionPool_test : public ::testing::Test {
  public:
    sql_BasicConnectionPool_test()
        : creator()
        , creator_ptr(&creator, no_delete)
        , pool(creator_ptr) {

    }

    virtual void SetUp() {
    }
    
    Connection* create_conn() {
        return new MockConnection();
    }
    
    MockConnectionCreator creator;
    shared_ptr<ConnectionCreator> creator_ptr;
    BasicConnectionPool pool;
};

TEST_F(sql_BasicConnectionPool_test, test_get) {
    auto_ptr<MockConnection> conn(new MockConnection());
    EXPECT_CALL(creator, do_create())
        .WillOnce(Return(conn.get()));
    EXPECT_CALL(*conn, do_is_open())
        .WillOnce(Return(false));
    EXPECT_CALL(*conn, do_open());
    
    shared_ptr<ConnectionProxy> cp =
        dynamic_pointer_cast<ConnectionProxy>(pool.get());
    conn.release();
    ASSERT_TRUE(cp);
    Connection* proxied = cp->proxied();
    cp.reset();
    
    cp = dynamic_pointer_cast<ConnectionProxy>(pool.get());
    ASSERT_TRUE(cp);
    EXPECT_EQ(proxied, cp->proxied());
}

TEST_F(sql_BasicConnectionPool_test, test_dtor) {
    shared_ptr<ConnectionProxy> cp;
    {
        BasicConnectionPool p(creator_ptr);
        p.put(new MockConnection());
        cp = dynamic_pointer_cast<ConnectionProxy>(p.get());
    }
    
    EXPECT_FALSE(cp->pool());
}

} // namespace sql
} // namespace brfc
