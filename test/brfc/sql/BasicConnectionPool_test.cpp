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

#include <boost/ref.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/FakeConnection.hpp>
#include <brfc/sql/MockConnectionCreator.hpp>
#include <brfc/sql/MockConnectionDeleter.hpp>
#include <brfc/util/Url.hpp>
#include <brfc/util/no_delete.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_BasicConnectionPool_test : public ::testing::Test {
  public:
    sql_BasicConnectionPool_test()
        : creator()
        , deleter()
        , pool(boost::ref(creator), no_delete, 2) {
    }

    virtual void SetUp() {
    }
    
    ::testing::StrictMock<MockConnectionCreator> creator;
    ::testing::StrictMock<MockConnectionDeleter> deleter;
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
    FakeConnection conn;

    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn));
    
    std::auto_ptr<Connection> c(pool.get());
    ConnectionProxy* cp = dynamic_cast<ConnectionProxy*>(c.get());
    
    ASSERT_TRUE(cp);
    EXPECT_EQ(&conn, cp->conn());
    EXPECT_EQ(1u, pool.size());
    EXPECT_EQ(1u, pool.lease_count());
}

TEST_F(sql_BasicConnectionPool_test, test_get_limit_reached) {
    FakeConnection conn1, conn2;

    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn1))
        .WillOnce(Return(&conn2));
     
    std::auto_ptr<Connection> c1(pool.get()); 
    std::auto_ptr<Connection> c2(pool.get());
    EXPECT_THROW(pool.get(), db_error);
    EXPECT_EQ(2u, pool.size());
    EXPECT_EQ(2u, pool.lease_count());
}

TEST_F(sql_BasicConnectionPool_test, test_size) {
    FakeConnection conn1;

    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn1));

    EXPECT_EQ(0u, pool.size());
    std::auto_ptr<Connection> c1(pool.get());
    EXPECT_EQ(1u, pool.size());
    c1.reset();
    EXPECT_EQ(1u, pool.size());
}

TEST_F(sql_BasicConnectionPool_test, test_dtor) {
    FakeConnection conn1;
    
    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn1));
    EXPECT_CALL(deleter, call(&conn1));

    std::auto_ptr<Connection> c1;
    {
        BasicConnectionPool p(boost::ref(creator), boost::ref(deleter));
        c1.reset(p.get());
    }
    EXPECT_FALSE(c1->is_open());
}

TEST_F(sql_BasicConnectionPool_test, test_put) {
    FakeConnection conn1;

    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn1));

    std::auto_ptr<Connection> c1(pool.get());
    c1.reset();
    
    EXPECT_EQ(1u, pool.size());
    EXPECT_EQ(0u, pool.lease_count());
}

TEST_F(sql_BasicConnectionPool_test, test_put_closed_conn) {
    FakeConnection conn1;
     
    EXPECT_CALL(creator, call())
        .WillOnce(Return(&conn1));

    std::auto_ptr<Connection> c1(pool.get());
    EXPECT_TRUE(c1.get());
    conn1.close();
    c1.reset();

    EXPECT_EQ(0u, pool.size());
    EXPECT_EQ(0u, pool.lease_count());
}

} // namespace sql
} // namespace brfc
