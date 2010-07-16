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
#include <gmock/gmock.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/rdb/Connection.hpp>
#include <brfc/rdb/Result.hpp>
#include <brfc/rdb/SqlQuery.hpp>

#include "../common.hpp"
#include "MockResult.hpp"
#include "MockConnection.hpp"

using testing::Return;
using testing::Throw;
using testing::_;

namespace brfc {
namespace rdb {

class rdb_Connection_test : public testing::Test {
  public:
    rdb_Connection_test()
        : conn() {
    }

    void SetUp() {
        ON_CALL(conn, do_is_open())
            .WillByDefault(Return(true));
    }

    ::testing::NiceMock<MockConnection> conn;
};

TEST_F(rdb_Connection_test, test_create_valid_url) {
    EXPECT_TRUE(Connection::create("postgresql://user:password@localhost/dbname"));
}

TEST_F(rdb_Connection_test, test_create_invalid_url) {
    EXPECT_THROW(Connection::create("bla://user:password@localhost/dbname"), value_error);
}

TEST_F(rdb_Connection_test, test_no_transaction_execute) {
    String stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false))  // execute()
        .WillOnce(Return(false))  // begin()
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_begin());
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResult>()));
    EXPECT_CALL(conn, do_commit());

    conn.execute(stmt);
}

TEST_F(rdb_Connection_test, test_no_transaction_excute_throws) {
    String stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false)) // execute()
        .WillOnce(Return(false)) // being()
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_begin());
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(conn, do_rollback());

    EXPECT_THROW(conn.execute(stmt), db_error);
}

TEST_F(rdb_Connection_test, test_in_transaction_execute) {
    String stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResult>()));
    
    conn.execute(stmt);
}

TEST_F(rdb_Connection_test, test_in_transaction_execute_throws) {
    String stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Throw(std::runtime_error("")));
    
    EXPECT_THROW(conn.execute(stmt), db_error);
}

TEST_F(rdb_Connection_test, test_variant_to_string_string) {
    EXPECT_EQ("'qweqwe'", conn.variant_to_string(Variant("qweqwe")));
}

TEST_F(rdb_Connection_test, test_variant_to_string_integer) {
    EXPECT_EQ("1", conn.variant_to_string(Variant(1)));
}

TEST_F(rdb_Connection_test, test_variant_to_string_float) {
    EXPECT_EQ("0.5", conn.variant_to_string(Variant(0.5)));
}

TEST_F(rdb_Connection_test, test_variant_to_string_date) {
    EXPECT_EQ("'2001-05-01'", conn.variant_to_string(Variant(Date(2001, 5, 1))));
}

TEST_F(rdb_Connection_test, test_variant_to_string_time) {
    EXPECT_EQ("'13:05:59.001'", conn.variant_to_string(Variant(Time(13, 5, 59, 1))));
}

TEST_F(rdb_Connection_test, test_variant_to_string_null) {
    EXPECT_EQ("NULL", conn.variant_to_string(Variant()));
}

TEST_F(rdb_Connection_test, test_begin) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));
    EXPECT_CALL(conn, do_begin());

    EXPECT_NO_THROW(conn.begin());
}

TEST_F(rdb_Connection_test, test_begin_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));

    EXPECT_THROW(conn.begin(), db_error);
}

TEST_F(rdb_Connection_test, test_begin_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.begin(), db_error);
}

TEST_F(rdb_Connection_test, test_rollback_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_rollback());

    EXPECT_NO_THROW(conn.rollback());
}

TEST_F(rdb_Connection_test, test_rollback_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.rollback(), db_error);
}

TEST_F(rdb_Connection_test, test_rollback_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.rollback(), db_error);
}

TEST_F(rdb_Connection_test, test_commit_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_commit());

    EXPECT_NO_THROW(conn.commit());
}

TEST_F(rdb_Connection_test, test_commit_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.commit(), db_error);
}

TEST_F(rdb_Connection_test, test_commit_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.commit(), db_error);
}

TEST_F(rdb_Connection_test, test_execute_sqlquery) {
    String stmt("query");
    sql::BindMap binds;
    SqlQuery query(stmt, binds);

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResult>()));
    
    conn.execute(query);
}

TEST_F(rdb_Connection_test, test_execute_replaces_binds) {
    String stmt(":bind");
    sql::BindMap binds;
    binds.add(":bind", Variant(1));

    ON_CALL(conn, do_execute(_))
        .WillByDefault(Return(shared_ptr<Result>()));

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(String("1")))
        .WillOnce(Return(shared_ptr<Result>()));
    
    conn.execute(stmt, binds);
}

} // namespace rdb
} // namespace brfc
