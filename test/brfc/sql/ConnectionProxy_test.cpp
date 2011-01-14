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

#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/ResultProxy.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockCompiler.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionPool.hpp>
#include <brfc/sql/MockDialect.hpp>
#include <brfc/sql/MockResult.hpp>

using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TypedEq;

namespace brfc {
namespace sql {

class sql_ConnectionProxy_test : public ::testing::Test {
  public:
    sql_ConnectionProxy_test()
            : pool()
            , conn_ptr(new MockConnection())
            , conn(*conn_ptr) 
            , proxy(ConnectionProxy::create(0, conn_ptr.release())) {
    }

    virtual void SetUp() {
        ON_CALL(conn, do_is_open())
            .WillByDefault(Return(true));
        ON_CALL(conn, do_in_transaction())
            .WillByDefault(Return(true));
    }
    
    MockConnectionPool pool;
    auto_ptr<MockConnection> conn_ptr;
    MockConnection& conn;
    shared_ptr<ConnectionProxy> proxy;
};

TEST_F(sql_ConnectionProxy_test, test_dtor) {
    auto_ptr<MockConnection> c(new MockConnection());
    MockConnection* cptr = c.get();
    EXPECT_CALL(pool, do_put(cptr));
    {
        shared_ptr<ConnectionProxy> p = ConnectionProxy::create(&pool, c.release());
    }
}

TEST_F(sql_ConnectionProxy_test, test_dtor_released) {
    auto_ptr<MockConnection> c(new MockConnection());
    {
        shared_ptr<ConnectionProxy> p = ConnectionProxy::create(&pool, c.release());
        p->release();
    }
}

TEST_F(sql_ConnectionProxy_test, test_release) {
    auto_ptr<MockConnection> c(new MockConnection());
    shared_ptr<ConnectionProxy> p = ConnectionProxy::create(&pool, c.release());
    p->release();
    EXPECT_FALSE(p->pool());
}

TEST_F(sql_ConnectionProxy_test, test_open) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    EXPECT_CALL(conn, do_open());

    proxy->open();
}

TEST_F(sql_ConnectionProxy_test, test_close) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_close());

    proxy->close();
}

TEST_F(sql_ConnectionProxy_test, test_is_open) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_TRUE(proxy->is_open());
    EXPECT_FALSE(proxy->is_open());
}

TEST_F(sql_ConnectionProxy_test, test_begin) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));
    EXPECT_CALL(conn, do_begin());

    proxy->begin();
}


TEST_F(sql_ConnectionProxy_test, test_commit) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_commit());

    proxy->commit();
}

TEST_F(sql_ConnectionProxy_test, test_rollback) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_rollback());

    proxy->rollback();
}

TEST_F(sql_ConnectionProxy_test, test_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true))
        .WillOnce(Return(false));
    
    EXPECT_TRUE(proxy->in_transaction());
    EXPECT_FALSE(proxy->in_transaction());
}

TEST_F(sql_ConnectionProxy_test, test_execute) {
    auto_ptr<Result> r(new MockResult());
    std::string stmt = "stmt";
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(r.release()));
    
    EXPECT_EQ(1, proxy.use_count());
    auto_ptr<Result> rr(proxy->execute(stmt));
    ASSERT_TRUE(rr.get());
    EXPECT_EQ(2, proxy.use_count());
    EXPECT_TRUE(dynamic_cast<ResultProxy*>(rr.get()));
}

TEST_F(sql_ConnectionProxy_test, test_dialect) {
    MockDialect d;    
    EXPECT_CALL(conn, do_dialect())
        .WillOnce(ReturnRef(d));
    
    EXPECT_EQ(&d, &proxy->dialect());
}

TEST_F(sql_ConnectionProxy_test, test_compiler) {
    MockCompiler c;
    EXPECT_CALL(conn, do_compiler())
        .WillOnce(ReturnRef(c));
    
    EXPECT_EQ(&c, &proxy->compiler());
}

TEST_F(sql_ConnectionProxy_test, test_store_large_object) {
    EXPECT_CALL(conn, do_store_large_object("/path/to/file"))
        .WillOnce(Return(1));
    
    EXPECT_EQ(1, proxy->store_large_object("/path/to/file"));
}

TEST_F(sql_ConnectionProxy_test, test_large_object_to_file) {
    EXPECT_CALL(conn, do_large_object_to_file(1, "/path/to/file"));
    
    proxy->large_object_to_file(1, "/path/to/file");
}

TEST_F(sql_ConnectionProxy_test, test_last_insert_id) {
    EXPECT_CALL(conn, do_last_insert_id())
        .WillOnce(Return(10));
    
    EXPECT_EQ(10, proxy->last_insert_id());
}

} // namespace sql
} // namespace brfc
