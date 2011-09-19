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

#include <boost/checked_delete.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionCreator.hpp>
#include <brfc/util/Url.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_BasicConnectionPool_test : public ::testing::Test {
  public:
    sql_BasicConnectionPool_test()
        : creator()
        , pool(boost::ref(creator), 2) {

    }

    virtual void SetUp() {
    }
    
    ::testing::StrictMock<MockConnectionCreator> creator;
    BasicConnectionPool pool;
};

TEST_F(sql_BasicConnectionPool_test, test_ctor_url) {
    Url url("scheme://user:password@host/database?pool_max_size=3");
    BasicConnectionPool p(url);
    EXPECT_EQ(3u, p.max_size());
}

TEST_F(sql_BasicConnectionPool_test, test_ctor_url_default) {
    Url url("scheme://user:password@host/database");
    BasicConnectionPool p(url);
    EXPECT_EQ(5u, p.max_size());
}

TEST_F(sql_BasicConnectionPool_test, test_ctor_url_invalid) {
    Url url("scheme://user:password@host/database?pool_max_size=-3");
    EXPECT_THROW(BasicConnectionPool p(url), std::invalid_argument);
}

TEST_F(sql_BasicConnectionPool_test, test_get) {
    MockConnection* conn = new MockConnection();
    ON_CALL(*conn, do_is_open())
        .WillByDefault(Return(true)); // for leak detection

    EXPECT_CALL(creator, call())
        .WillOnce(Return(conn));
    
    std::auto_ptr<Connection> c(pool.get());
    ConnectionProxy* cp = dynamic_cast<ConnectionProxy*>(c.get());
    
    // "destroyed" using put
    EXPECT_TRUE(cp->connection_dtor() == boost::bind(&BasicConnectionPool::put, &pool, _1));
    ASSERT_TRUE(cp);
    Connection* proxied = &cp->proxied();

    c.reset(); // put back to pool
    
    // same connection
    c.reset(pool.get());
    cp = dynamic_cast<ConnectionProxy*>(c.get());
    ASSERT_TRUE(cp);
    EXPECT_EQ(proxied, &cp->proxied());
}

TEST_F(sql_BasicConnectionPool_test, test_get_limit_reached) {
    MockConnection* conn1 = new MockConnection();
    MockConnection* conn2 = new MockConnection();
    ON_CALL(*conn1, do_is_open())
        .WillByDefault(Return(true)); // for leak detection
    ON_CALL(*conn2, do_is_open())
        .WillByDefault(Return(true)); // for leak detection

    EXPECT_CALL(creator, call())
        .WillOnce(Return(conn1))
        .WillOnce(Return(conn2));
     
    std::auto_ptr<Connection> c1(pool.get()); 
    std::auto_ptr<Connection> c2(pool.get());
    EXPECT_THROW(pool.get(), db_error);
    c2.reset();
    EXPECT_NO_THROW(c2.reset(pool.get()));
}

TEST_F(sql_BasicConnectionPool_test, test_size) {
    MockConnection* conn1 = new MockConnection();
    ON_CALL(*conn1, do_is_open())
        .WillByDefault(Return(true)); // for leak detection

    EXPECT_CALL(creator, call())
        .WillOnce(Return(conn1));

    EXPECT_EQ(0u, pool.size());
    std::auto_ptr<Connection> c1(pool.get());
    EXPECT_EQ(1u, pool.size());
    c1.reset();
    EXPECT_EQ(1u, pool.size());
}

TEST_F(sql_BasicConnectionPool_test, test_dtor) {
    MockConnection* conn1 = new MockConnection();
    ON_CALL(*conn1, do_is_open())
        .WillByDefault(Return(true)); // for leak detection
    
    EXPECT_CALL(creator, call())
        .WillOnce(Return(conn1));

    std::auto_ptr<Connection> c1;
    {
        BasicConnectionPool p(boost::ref(creator));
        c1.reset(p.get());
    }
}

TEST_F(sql_BasicConnectionPool_test, test_put_closed_conn) {
    MockConnection* conn1 = new MockConnection();
     
    EXPECT_CALL(creator, call())
        .WillOnce(Return(conn1));
    EXPECT_CALL(*conn1, do_is_open())
        .WillOnce(Return(false));

    std::auto_ptr<Connection> c1(pool.get());
    c1.reset();

    EXPECT_EQ(0u, pool.size()); // has gone through dispose();
}

} // namespace sql
} // namespace brfc
