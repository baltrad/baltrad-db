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

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>

#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../../common.hpp"
#include "../../MockHasher.hpp"

using ::testing::_;
using ::testing::Return;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RdbHelper_test : public ::testing::TestWithParam<const char*> {
  public:
    db_rdb_RdbHelper_test()
            : db(TestRDBEnv::get_database(GetParam()))
            , hasher()
            , helper(db)
            , entry(db)
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Karlskrona") {
    }

    virtual void SetUp() {
        db->file_hasher(&hasher);
        ON_CALL(hasher, do_hash(_)).WillByDefault(Return("hash"));
    }

    virtual void TearDown() {
        db->clean();
    }

    test::TestRDB* db;
    ::testing::NiceMock<MockHasher> hasher;
    RdbHelper helper;
    RdbFileEntry entry;
    oh5::hl::HlFile file;
};


TEST_P(db_rdb_RdbHelper_test, test_insert_file) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    EXPECT_GT(entry.id(), 0);
    EXPECT_GT(entry.source_id(), 0);
    EXPECT_GT(static_cast<RdbFileEntry*>(entry.root().file())->id(), 0);
}

TEST_P(db_rdb_RdbHelper_test, test_insert_file_content) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    test::TempH5File tf;
    tf.write(file);
 
    db->conn().begin();
    try {
        EXPECT_NO_THROW(helper.insert_file_content(entry, tf.path()));
        EXPECT_GT(entry.lo_id(), 0);
    } catch (...) {
        db->conn().rollback();
    }
    EXPECT_NO_THROW(db->conn().commit());
}

TEST_P(db_rdb_RdbHelper_test, test_insert_group) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    oh5::Group& grp = entry.root();
    EXPECT_NO_THROW(helper.insert_group(grp));
    EXPECT_GT(helper.backend(grp).id(), 0);
    EXPECT_TRUE(helper.backend(grp).loaded());

    EXPECT_ANY_THROW(helper.insert_group(file.root()));
}

TEST_P(db_rdb_RdbHelper_test, test_insert_attribute) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    oh5::Group& grp = entry.root();

}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_rdb_RdbHelper_test_p,
                        db_rdb_RdbHelper_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace db
} // namespace brfc
