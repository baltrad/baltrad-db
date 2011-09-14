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

#include <boost/scoped_ptr.hpp>

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/oh5/Oh5PhysicalFile.hpp>
#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>
#include <brfc/rdb/RdbInDatabaseFileStorage.hpp>
#include <brfc/test/TestRDB.hpp>
#include <brfc/util/BoostFileSystem.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class rdb_RdbInDatabaseFileStorage_itest :
        public ::testing::TestWithParam<const char*> {
  public:
    rdb_RdbInDatabaseFileStorage_itest()
            : db(ITestEnv::get_database(GetParam()))
            , storage()
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "WMO:02666")
            , temp_file() {

    }

    virtual void SetUp() {
        storage.database(db);
        Oh5HlFileWriter writer;
        writer.write(file, temp_file.path());
        file.path(temp_file.path());
    }
    
    virtual void TearDown() {
        db->clean();
    }

    test::TestRDB* db;
    RdbInDatabaseFileStorage storage;
    Oh5PhysicalFile file;
    NamedTemporaryFile temp_file;
};

TEST_P(rdb_RdbInDatabaseFileStorage_itest, test_store) {
    boost::scoped_ptr<RdbFileEntry> e(db->file_to_entry(file));
    storage.store(*e, file.path());

    EXPECT_GT(e->id(), 0);
    EXPECT_GT(e->lo_id(), 0);
}

TEST_P(rdb_RdbInDatabaseFileStorage_itest, test_retrieve) {
    boost::scoped_ptr<RdbFileEntry> e(db->file_to_entry(file));
    storage.store(*e, file.path());
    
    NamedTemporaryFile tf;
    storage.retrieve(*e, tf.path());
    
    BoostFileSystem fsys;
    long long file_size = fsys.file_size(tf.path());
    EXPECT_GT(file_size, 0);
    EXPECT_EQ(e->size(), file_size);
}

TEST_P(rdb_RdbInDatabaseFileStorage_itest, test_remove) {
    boost::scoped_ptr<RdbFileEntry> e(db->file_to_entry(file));
    storage.store(*e, file.path());

    EXPECT_TRUE(storage.remove(*e));
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RdbInDatabaseFileStorage_itest_p,
                        rdb_RdbInDatabaseFileStorage_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
