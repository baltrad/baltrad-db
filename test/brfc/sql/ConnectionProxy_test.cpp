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
#include <brfc/sql/LargeObjectProxy.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockCompiler.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionPool.hpp>
#include <brfc/sql/MockDialect.hpp>
#include <brfc/sql/MockLargeObject.hpp>
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
            , conn() 
            , proxy(ConnectionProxy::create(&pool, &conn)) {
    }

    virtual void SetUp() {
        ON_CALL(conn, do_is_open())
            .WillByDefault(Return(true));
        ON_CALL(conn, do_in_transaction())
            .WillByDefault(Return(true));
        EXPECT_CALL(pool, do_put(&conn));
    }
    
    MockConnectionPool pool;
    ::testing::NiceMock<MockConnection> conn;
    shared_ptr<ConnectionProxy> proxy;
};

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
    shared_ptr<Result> r = make_shared<MockResult>();
    String stmt = "stmt";
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(r));
    
    EXPECT_EQ(1, proxy.use_count());
    shared_ptr<Result> rr = proxy->execute(stmt);
    ASSERT_TRUE(rr);
    EXPECT_EQ(2, proxy.use_count());
    EXPECT_TRUE(dynamic_pointer_cast<ResultProxy>(rr));
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

TEST_F(sql_ConnectionProxy_test, test_large_object_longlong) {
    shared_ptr<LargeObject> lo = make_shared<MockLargeObject>();
    EXPECT_CALL(conn, do_large_object(1))
        .WillOnce(Return(lo));
    
    EXPECT_EQ(1, proxy.use_count());
    shared_ptr<LargeObject> rlo = proxy->large_object(1);
    ASSERT_TRUE(rlo);
    EXPECT_EQ(2, proxy.use_count());
    EXPECT_TRUE(dynamic_pointer_cast<LargeObjectProxy>(rlo));
}

TEST_F(sql_ConnectionProxy_test, test_large_object_string) {
    shared_ptr<LargeObject> lo = make_shared<MockLargeObject>();
    EXPECT_CALL(conn, do_large_object(TypedEq<const String&>("path")))
        .WillOnce(Return(lo));
    
    EXPECT_EQ(1, proxy.use_count());
    shared_ptr<LargeObject> rlo = proxy->large_object("path");
    ASSERT_TRUE(rlo);
    EXPECT_EQ(2, proxy.use_count());
    EXPECT_TRUE(dynamic_pointer_cast<LargeObjectProxy>(rlo));
}

TEST_F(sql_ConnectionProxy_test, test_last_insert_id) {
    EXPECT_CALL(conn, do_last_insert_id())
        .WillOnce(Return(10));
    
    EXPECT_EQ(10, proxy->last_insert_id());
}

} // namespace sql
} // namespace brfc
