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

#include <boost/filesystem.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/oh5/hl/HlFile.hpp>


#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../../common.hpp"

using testing::_;
using testing::Return;

namespace fs = boost::filesystem;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RelationalDatabase_test : public testing::TestWithParam<const char*> {
  public:
    db_rdb_RelationalDatabase_test()
            : db(TestRDBEnv::get_database(GetParam())) {
    }

    virtual void TearDown() {
        db->clean();
    }

    test::TestRDB* db;
};

TEST_P(db_rdb_RelationalDatabase_test, store) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());

    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->store(file));
    shared_ptr<RdbFileEntry> re =
        dynamic_pointer_cast<RdbFileEntry>(e);
    ASSERT_TRUE(re);
    EXPECT_TRUE(re->id() > 0);
    EXPECT_TRUE(re->lo_id() > 0);
    EXPECT_FALSE(e->source().empty());
    EXPECT_TRUE(e->source().has("name"));
}

TEST_P(db_rdb_RelationalDatabase_test, entry_by_uuid) {
    EXPECT_THROW(db->entry_by_uuid("nxuuid"), lookup_error);

    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e1, e2;
    ASSERT_NO_THROW(e1 = db->store(file));
     
    ASSERT_NO_THROW(e2 = db->entry_by_uuid(e1->uuid()));

    EXPECT_EQ(e1->uuid(), e2->uuid());
    EXPECT_EQ(e1->hash(), e2->hash());
}

TEST_P(db_rdb_RelationalDatabase_test, remove) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->store(file));
    ASSERT_TRUE(e);

    bool removed = false;
    EXPECT_NO_THROW(removed = db->remove(*e));
    EXPECT_TRUE(removed);

    EXPECT_NO_THROW(removed = db->remove(*e));
    EXPECT_FALSE(removed);
}

//XXX: this should be tested somewhere else?
TEST_P(db_rdb_RelationalDatabase_test, write_entry_to_file) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->store(file));
    ASSERT_TRUE(e);
    
    // test writing
    test::TempH5File tef;
    EXPECT_NO_THROW(e->write_to_file(tef.path()));
    EXPECT_EQ(fs::file_size(tef.path().to_utf8()), fs::file_size(tf.path().to_utf8()));
}

TEST_P(db_rdb_RelationalDatabase_test, store_with_invalid_attributes) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    // add an invalid attribute
    file.root().create_attribute("invalid", oh5::Scalar(1));

    EXPECT_NO_THROW(db->store(file));
}

TEST_P(db_rdb_RelationalDatabase_test, attribute_groups_not_saved) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    ASSERT_NO_THROW(db->store(file));
    
    /* XXX: figure out how to test this
    EXPECT_EQ(0, file->root()->child_group_by_name("what")->db_id());
    EXPECT_NE(0, file->root()->db_id());
    */
}

TEST_P(db_rdb_RelationalDatabase_test, DISABLED_resultset_keeps_qsqldatabase_alive) {
    shared_ptr<sql::Result> r;
    {
        RelationalDatabase db(GetParam());
        r = db.conn()->execute("SELECT 1");
    };
    r->size();
}


#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_rdb_RelationalDatabase_test_p,
                        db_rdb_RelationalDatabase_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace db
} // namespace brfc
