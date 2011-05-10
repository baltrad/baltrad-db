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

#include <brfc/exceptions.hpp>
#include <brfc/expr/Listcons.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Result.hpp>
#include <brfc/sql/Select.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockCompiler.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockDialect.hpp>
#include <brfc/sql/MockResult.hpp>

using testing::Ref;
using testing::Return;
using testing::ReturnRef;
using testing::Throw;
using testing::_;
using ::brfc::expr::Expression;
using ::brfc::expr::Listcons;

namespace brfc {
namespace sql {

class sql_Connection_test : public testing::Test {
  public:
    sql_Connection_test()
        : compiler()
        , dialect()
        , conn()
        , xpr() {
    }

    void SetUp() {
        dialect.delegate_to_fake();
        ON_CALL(conn, do_is_open())
            .WillByDefault(Return(true));
        ON_CALL(conn, do_dialect())
            .WillByDefault(ReturnRef(dialect));
        ON_CALL(conn, do_compiler())
            .WillByDefault(ReturnRef(compiler));
    }

    ::testing::NiceMock<MockCompiler> compiler;
    ::testing::NiceMock<MockDialect> dialect;
    ::testing::NiceMock<MockConnection> conn;
    Factory xpr;
};

TEST_F(sql_Connection_test, test_close) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_close());
    
    conn.close();
}

TEST_F(sql_Connection_test, test_close_on_closed) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));

    EXPECT_NO_THROW(conn.close());
}

TEST_F(sql_Connection_test, test_no_transaction_execute) {
    MockResult result;
    std::string stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false))  // execute()
        .WillOnce(Return(false))  // begin()
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_begin());
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(&result));
    EXPECT_CALL(conn, do_commit());

    EXPECT_EQ(&result, conn.execute(stmt));
}

TEST_F(sql_Connection_test, test_no_transaction_excute_throws) {
    std::string stmt("query");
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

TEST_F(sql_Connection_test, test_in_transaction_execute) {
    MockResult result;
    std::string stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(&result));
    
    EXPECT_EQ(&result, conn.execute(stmt));
}

TEST_F(sql_Connection_test, test_in_transaction_execute_throws) {
    std::string stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Throw(std::runtime_error("")));
    
    EXPECT_THROW(conn.execute(stmt), db_error);
}

TEST_F(sql_Connection_test, test_begin) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));
    EXPECT_CALL(conn, do_begin());

    EXPECT_NO_THROW(conn.begin());
}

TEST_F(sql_Connection_test, test_begin_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));

    EXPECT_THROW(conn.begin(), db_error);
}

TEST_F(sql_Connection_test, test_begin_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.begin(), db_error);
}

TEST_F(sql_Connection_test, test_rollback_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_rollback());

    EXPECT_NO_THROW(conn.rollback());
}

TEST_F(sql_Connection_test, test_rollback_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.rollback(), db_error);
}

TEST_F(sql_Connection_test, test_rollback_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.rollback(), db_error);
}

TEST_F(sql_Connection_test, test_commit_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_commit());

    EXPECT_NO_THROW(conn.commit());
}

TEST_F(sql_Connection_test, test_commit_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.commit(), db_error);
}

TEST_F(sql_Connection_test, test_commit_on_closed_connection) {
    EXPECT_CALL(conn, do_is_open())
        .WillOnce(Return(false));
    
    EXPECT_THROW(conn.commit(), db_error);
}

TEST_F(sql_Connection_test, test_execute_sqlquery) {
    std::string stmt("query");
    MockResult result;

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(&result));
    
    EXPECT_EQ(&result, conn.execute(stmt));
}

TEST_F(sql_Connection_test, test_execute_insert) {
    Insert query("t1");
    Expression compiled = Listcons().string("query").get();
    MockResult result;

    EXPECT_CALL(compiler, do_compile(query.expression()))
        .WillOnce(Return(compiled));
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute("query"))
        .WillOnce(Return(&result));
    
    EXPECT_EQ(&result, conn.execute(query));
}

TEST_F(sql_Connection_test, test_execute_select) {
    Select query;
    Expression compiled = Listcons().string("query").get();
    MockResult result;

    EXPECT_CALL(compiler, do_compile(query.expression()))
        .WillOnce(Return(compiled));
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute("query"))
        .WillOnce(Return(&result));
    
    EXPECT_EQ(&result, conn.execute(query));
}

TEST_F(sql_Connection_test, test_execute_replaces_binds) {
    Expression stmt = Listcons().string("query ").append(xpr.bind("b")).get();
    Connection::BindMap_t binds;
    binds["b"] = expr::Expression(1);
    MockResult result;

    ON_CALL(conn, do_execute(_))
        .WillByDefault(Return(&result));

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute("query 1"))
        .WillOnce(Return(&result));
    
    conn.execute(stmt, binds);
}

} // namespace sql
} // namespace brfc
