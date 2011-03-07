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

#include <brfc/sql/ResultProxy.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/test_common.hpp>
#include <brfc/sql/MockResult.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionDtor.hpp>
#include <brfc/sql/ConnectionProxy.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_ResultProxy_test : public ::testing::Test {
  public:
    sql_ResultProxy_test()
            : result_ptr(new MockResult())
            , result(*result_ptr)
            , conn()
            , conn_dtor()
            , conn_dtor_ptr(&conn_dtor, no_delete)
            , connproxy(&conn, conn_dtor_ptr)
            , proxy(&connproxy, result_ptr.release()) {
    
    }

    virtual void SetUp() {
        EXPECT_CALL(conn_dtor, do_destroy(&conn));
    }

    auto_ptr<MockResult> result_ptr;
    MockResult& result;
    MockConnection conn;
    MockConnectionDtor conn_dtor;
    shared_ptr<MockConnectionDtor> conn_dtor_ptr;
    ConnectionProxy connproxy;
    ResultProxy proxy;
};

TEST_F(sql_ResultProxy_test, test_close) {
    EXPECT_CALL(result, do_close());

    proxy.close();
}

TEST_F(sql_ResultProxy_test, test_closed_proxy_throws) {
    EXPECT_CALL(result, do_close());

    proxy.close();

    EXPECT_THROW(proxy.next(), db_error);
    EXPECT_THROW(proxy.seek(1), db_error);
    EXPECT_THROW(proxy.size(), db_error);
    EXPECT_THROW(proxy.value_at(0), db_error);
    EXPECT_THROW(proxy.value_at("column"), db_error);
    EXPECT_THROW(proxy.affected_rows(), db_error);
}

TEST_F(sql_ResultProxy_test, test_next) {
    EXPECT_CALL(result, do_next())
        .WillOnce(Return(true));
    
    EXPECT_TRUE(proxy.next());
}

TEST_F(sql_ResultProxy_test, test_seek) {
    EXPECT_CALL(result, do_seek(10))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(proxy.seek(10));
}

TEST_F(sql_ResultProxy_test, test_size) {
    EXPECT_CALL(result, do_size())
        .WillOnce(Return(2));
    
    EXPECT_EQ(2, proxy.size());
}

TEST_F(sql_ResultProxy_test, test_value_at) {
    Variant v(10);
    EXPECT_CALL(result, do_value_at(1))
        .WillOnce(Return(v));
    
    EXPECT_EQ(v, proxy.value_at(1));
}

} // namespace sql
} // namespace brfc
