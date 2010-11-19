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

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/sql/DialectCompiler.hpp>

#include "../../sql/MockDialect.hpp"
#include "../../sql/MockConnection.hpp"
#include "../../sql/MockConnectionPool.hpp"
#include "../../oh5/MockPhysicalFile.hpp"
#include "../../sql/MockResult.hpp"
#include "../MockFileEntry.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RelationalDatabase_test2 : public ::testing::Test {
  public:
    db_rdb_RelationalDatabase_test2()
            : conn()
            , conn_ptr(&conn, no_delete)
            , dialect()
            , compiler(&dialect)
            , pool()
            , pool_ptr(&pool, no_delete)
            , result()
            , result_ptr(&result, no_delete)
            , rdb(pool_ptr) {
    }

    virtual void SetUp() {
        conn.delegate_to_fake();
        conn.open();
        ON_CALL(conn, do_dialect())
            .WillByDefault(ReturnRef(dialect));
        ON_CALL(conn, do_compiler())
            .WillByDefault(ReturnRef(compiler));
    }
    
    ::testing::NiceMock<sql::MockConnection> conn;
    shared_ptr<sql::Connection> conn_ptr;
    ::testing::NiceMock<sql::MockDialect> dialect;
    sql::DialectCompiler compiler;
    sql::MockConnectionPool pool;
    shared_ptr<sql::ConnectionPool> pool_ptr;
    sql::MockResult result;
    shared_ptr<sql::Result> result_ptr;
    RelationalDatabase rdb;
};

TEST_F(db_rdb_RelationalDatabase_test2, test_execute_attribute_query) {
    EXPECT_CALL(pool, do_get())
        .WillOnce(Return(conn_ptr));
    EXPECT_CALL(conn, do_execute(_))
        .WillOnce(Return(result_ptr));

    AttributeQuery q;
    shared_ptr<AttributeResult> r = rdb.execute(q);
}

TEST_F(db_rdb_RelationalDatabase_test2, test_execute_file_query) {
    EXPECT_CALL(pool, do_get())
        .WillOnce(Return(conn_ptr));
    EXPECT_CALL(conn, do_execute(_))
        .WillOnce(Return(result_ptr));

    FileQuery q;
    shared_ptr<FileResult> r = rdb.execute(q);
}

TEST_F(db_rdb_RelationalDatabase_test2, test_remove) {
    MockFileEntry e;
    EXPECT_CALL(pool, do_get())
        .WillOnce(Return(conn_ptr));
    EXPECT_CALL(e, do_uuid())
        .WillOnce(Return("abcdefg"));
    EXPECT_CALL(conn, do_execute(_)) // check statement
        .WillOnce(Return(result_ptr));
    EXPECT_CALL(result, do_affected_rows())
        .WillOnce(Return(1));

    EXPECT_TRUE(rdb.remove(e));
}

} // namespace rdb
} // namespace db
} // namespace brfc
