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
#include <brfc/itest_config.hpp>
#include <brfc/test_common.hpp>
#include <brfc/sql/pg/Connection.hpp>
#include <brfc/util/Url.hpp>

namespace brfc {
namespace sql {
namespace pg {

class sql_pg_Connection_itest : public testing::TestWithParam<const char*> {
  public:
    sql_pg_Connection_itest()
            : conn(Url(std::string(GetParam()))) {
    }
    
    Connection conn;
};

TEST_P(sql_pg_Connection_itest, test_close) {
    EXPECT_TRUE(conn.is_open());
    conn.close();
    EXPECT_FALSE(conn.is_open());
}

TEST_P(sql_pg_Connection_itest, test_close_on_closed) {
    EXPECT_TRUE(conn.is_open());
    conn.close();
    conn.close();
    EXPECT_FALSE(conn.is_open());
}

TEST_P(sql_pg_Connection_itest, test_begin) {
    EXPECT_FALSE(conn.in_transaction());
    conn.begin();
    EXPECT_TRUE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_begin_in_transaction) {
    EXPECT_FALSE(conn.in_transaction());
    conn.begin();
    EXPECT_THROW(conn.begin(), db_error);
    EXPECT_TRUE(conn.in_transaction());   
}

TEST_P(sql_pg_Connection_itest, test_begin_on_closed_connection) {
    conn.close();
    EXPECT_THROW(conn.begin(), db_error);
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_rollback_in_transaction) {
    conn.begin();
    conn.rollback();
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_rollback_no_transaction) {
    EXPECT_THROW(conn.rollback(), db_error);
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_rollback_on_closed_connection) {
    conn.close();
    EXPECT_THROW(conn.rollback(), db_error);
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_commit_in_transaction) {
    conn.begin();
    conn.commit();
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_commit_no_transaction) {
    EXPECT_THROW(conn.commit(), db_error);
    EXPECT_FALSE(conn.in_transaction());
}

TEST_P(sql_pg_Connection_itest, test_commit_on_closed_connection) {
    conn.close();
    EXPECT_THROW(conn.commit(), db_error);
    EXPECT_FALSE(conn.in_transaction());
}

std::vector<const char*>
filter_pg_dsns(const char** dsns) {
    std::vector<const char*> result;
    for (int i=0; i < BRFC_TEST_DSN_COUNT; ++i) {
        if (strncmp("postgresql://", dsns[i], 13) == 0) {
            result.push_back(dsns[i]);
        }
    }
    return result;
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(sql_pg_Connection_itest_p,
                        sql_pg_Connection_itest,
                        ::testing::ValuesIn(filter_pg_dsns(TEST_DSNS)));
#endif // BRFC_TEST_DSN_COUNT

} // namespace pg
} // namespace sql
} // namespace brfc
