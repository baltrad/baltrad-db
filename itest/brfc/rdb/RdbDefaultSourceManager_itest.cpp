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

#include <stdexcept>

#include <brfc/rdb/RdbDefaultSourceManager.hpp>

#include <brfc/oh5/Oh5Source.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/test/TestRDB.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class rdb_RdbDefaultSourceManager_itest : public ::testing::TestWithParam<const char*> {
  public:
    rdb_RdbDefaultSourceManager_itest()
            : db(ITestEnv::get_database(GetParam()))
            , conn(db->conn())
            , manager(conn) {
    }

    virtual void SetUp() {
        conn->begin();
    }

    virtual void TearDown() {
        if (conn->in_transaction())
            conn->rollback();
        db->clean();
    }

    test::TestRDB* db;
    boost::shared_ptr<sql::Connection> conn;
    RdbDefaultSourceManager manager;
};

TEST_P(rdb_RdbDefaultSourceManager_itest, test_source_by_id) {
    long long src_id = 0;
    Oh5Source src = Oh5Source::from_string("PLC:Legionowo");
    EXPECT_NO_THROW(src_id = manager.source_id(src));
    EXPECT_GT(src_id, 0);
    
    src.clear();
    EXPECT_NO_THROW(src = manager.source_by_id(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.get("RAD"));
    ASSERT_TRUE(src.has("_name"));
    EXPECT_EQ("plleg", src.get("_name"));
}

TEST_P(rdb_RdbDefaultSourceManager_itest, test_source_by_id_nx) {
    Oh5Source src;
    EXPECT_NO_THROW(manager.source_by_id(-1));
    EXPECT_TRUE(src.empty());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RdbDefaultSourceManager_itest_p,
                        rdb_RdbDefaultSourceManager_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
