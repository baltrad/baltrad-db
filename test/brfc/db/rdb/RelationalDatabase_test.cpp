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

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/ResultSet.hpp>

#include <brfc/db/rdb/RdbFileEntry.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/oh5/hl/HlFile.hpp>


#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../../common.hpp"
#include "../../MockHasher.hpp"

using testing::_;
using testing::Return;

namespace fs = boost::filesystem;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RelationalDatabase_test : public testing::TestWithParam<const char*> {
  public:
    db_rdb_RelationalDatabase_test()
            : db(TestRDBEnv::get_database(GetParam()))
            , hasher() {
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
    oh5::Source src;
};


TEST_P(db_rdb_RelationalDatabase_test, load_source_by_plc) {
    EXPECT_NO_THROW(src = db->load_source("PLC:Legionowo"));
    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.at("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.at("RAD"));
    ASSERT_TRUE(src.has("name"));
    EXPECT_EQ("plleg", src.at("name"));
}

TEST_P(db_rdb_RelationalDatabase_test, load_source_by_plc_unicode) {
    EXPECT_NO_THROW(src = db->load_source(String::from_utf8("PLC:Świdwin")));
    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ(String::from_utf8("Świdwin"), src.at("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL46", src.at("RAD"));
    ASSERT_TRUE(src.has("name"));
    EXPECT_EQ("plswi", src.at("name"));
}

/*
TEST_P(db_rdb_RelationalDatabase_test, load_same_source) {
    shared_ptr<oh5::SourceRadar> src1 =
        dynamic_pointer_cast<oh5::SourceRadar>(db->load_source("PLC:Legionowo"));
    shared_ptr<oh5::SourceRadar> src2 =
        dynamic_pointer_cast<oh5::SourceRadar>(db->load_source("PLC:Legionowo"));
    ASSERT_TRUE(src1);
    ASSERT_TRUE(src2);
    EXPECT_EQ(src1, src2);
    EXPECT_TRUE(src1->centre());
    EXPECT_TRUE(src2->centre());
    EXPECT_EQ(src1->centre(), src2->centre());
}
*/

/*
TEST_P(db_rdb_RelationalDatabase_test, load_radars_with_same_centre) {
    shared_ptr<oh5::SourceRadar> src1 =
        dynamic_pointer_cast<oh5::SourceRadar>(db->load_source("PLC:Legionowo"));
    shared_ptr<oh5::SourceRadar> src2 =
        dynamic_pointer_cast<oh5::SourceRadar>(db->load_source("PLC:Pastewnik"));
    ASSERT_TRUE(src1);
    ASSERT_TRUE(src2);
    EXPECT_NE(src1, src2);
    EXPECT_TRUE(src1->centre());
    EXPECT_TRUE(src2->centre());
    EXPECT_EQ(src1->centre(), src2->centre());
}
*/

TEST_P(db_rdb_RelationalDatabase_test, save_file) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->save_file(file));
    shared_ptr<RdbFileEntry> re =
        dynamic_pointer_cast<RdbFileEntry>(e);
    ASSERT_TRUE(re);
    EXPECT_TRUE(re->id() > 0);
    EXPECT_TRUE(re->lo_id() > 0);
}

TEST_P(db_rdb_RelationalDatabase_test, remove_file) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->save_file(file));

    bool removed = false;
    EXPECT_NO_THROW(removed = db->remove_file(*e));
    EXPECT_TRUE(removed);

    EXPECT_NO_THROW(removed = db->remove_file(*e));
    EXPECT_FALSE(removed);
}

//XXX: this should be tested somewhere else?
TEST_P(db_rdb_RelationalDatabase_test, write_entry_to_file) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    shared_ptr<FileEntry> e;
    EXPECT_NO_THROW(e = db->save_file(file));
    
    // test writing
    test::TempH5File tef;
    EXPECT_NO_THROW(e->write_to_file(tef.path()));
    EXPECT_EQ(fs::file_size(tef.path().to_utf8()), fs::file_size(tf.path().to_utf8()));
}

TEST_P(db_rdb_RelationalDatabase_test, save_file_with_invalid_attributes) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    // add an invalid attribute
    file.root().create_attribute("invalid", oh5::Scalar(1));

    EXPECT_NO_THROW(db->save_file(file));
}

TEST_P(db_rdb_RelationalDatabase_test, attribute_groups_not_saved) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    ASSERT_NO_THROW(db->save_file(file));
    
    /* XXX: figure out how to test this
    EXPECT_EQ(0, file->root()->child_group_by_name("what")->db_id());
    EXPECT_NE(0, file->root()->db_id());
    */
}

TEST_P(db_rdb_RelationalDatabase_test, resultset_keeps_qsqldatabase_alive) {
    shared_ptr<sql::Result> r;
    {
        RelationalDatabase db(GetParam());
        r = db.connection().execute("SELECT 1", sql::BindMap());
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
