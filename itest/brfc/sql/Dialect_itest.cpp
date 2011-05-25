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

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>

#include <brfc/test/TestRDB.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

namespace brfc {
namespace sql {

class sql_Dialect_itest : public testing::TestWithParam<const char*> {
  public:
    sql_Dialect_itest()
        : db(ITestEnv::get_database(GetParam()))
        , conn(db->conn())
        , dialect(conn->dialect()) {

    }
    
    test::TestRDB* db;
    boost::shared_ptr<Connection> conn;
    const Dialect& dialect;
};

TEST_P(sql_Dialect_itest, test_escape) {
    EXPECT_EQ("''", dialect.escape("'"));
    EXPECT_EQ("\\\\", dialect.escape("\\"));
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(sql_Dialect_itest_p,
                        sql_Dialect_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace sql
} // namespace brfc
