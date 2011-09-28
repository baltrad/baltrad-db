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

#include <pqxx/connection>

#include <brfc/exceptions.hpp>
#include <brfc/sql/pg/Connection.hpp>
#include <brfc/sql/pg/MockDbtransaction.hpp>
#include <brfc/util/Url.hpp>

using ::testing::StrEq;
using ::testing::Throw;

namespace brfc {
namespace sql {
namespace pg {

class sql_pg_Connection_test : public ::testing::Test {
  public:
    sql_pg_Connection_test()
            : pqxx_conn()
            , conn(&pqxx_conn) {
    }
    
    pqxx::nullconnection pqxx_conn;
    Connection conn;
};

TEST_F(sql_pg_Connection_test, test_ctor_no_connection) {
    Url url("postgresql://user:password@unknown-host/dbase");
    EXPECT_THROW(Connection c(url), db_error);
}

TEST_F(sql_pg_Connection_test, test_url_to_pg) {
    Url url("postgresql://myuser:mypass@myhost/mydbase?arg1=val1");
    EXPECT_EQ("host=myhost user=myuser password=mypass dbname=mydbase",
              Connection::url_to_pg(url));
}

TEST_F(sql_pg_Connection_test, test_exec_on_broken_connection) {
    pqxx::MockDbtransaction tx(pqxx_conn);
    conn.pqxx_transaction(&tx);
    
    EXPECT_CALL(tx, do_exec(StrEq("stmt")))
        .WillOnce(Throw(pqxx::broken_connection("")));
    
    EXPECT_THROW(conn.execute("stmt"), db_error);
    EXPECT_FALSE(conn.has_pqxx_connection());
}

TEST_F(sql_pg_Connection_test, test_exec_on_sql_error) {
    pqxx::MockDbtransaction tx(pqxx_conn);
    conn.pqxx_transaction(&tx);
    
    EXPECT_CALL(tx, do_exec(StrEq("stmt")))
        .WillOnce(Throw(pqxx::sql_error("")));
    
    EXPECT_THROW(conn.execute("stmt"), db_error);
    EXPECT_TRUE(conn.has_pqxx_connection());
}

} // namespace pg
} // namespace sql
} // namespace brfc
