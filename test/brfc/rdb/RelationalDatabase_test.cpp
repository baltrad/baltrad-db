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

#include <brfc/Date.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../common.hpp"
#include "../MockHasher.hpp"

using testing::_;
using testing::Return;


namespace brfc {
namespace rdb {

class rdb_RelationalDatabase_test : public testing::TestWithParam<const char*> {
  public:
    rdb_RelationalDatabase_test()
            : db(TestRDBEnv::get_database(GetParam()))
            , hasher() {
    }

    virtual void SetUp() {
        db->file_hasher(&hasher);
        ON_CALL(hasher, do_name()).WillByDefault(Return("mock"));
        ON_CALL(hasher, do_hash(_, _)).WillByDefault(Return("hash"));
    }

    virtual void TearDown() {
        db->clean();
        db->file_hasher(0);
    }

    test::TestRDB* db;
    ::testing::NiceMock<MockHasher> hasher;
};


TEST_P(rdb_RelationalDatabase_test, load_source_by_plc) {
    shared_ptr<oh5::Source> src = db->load_source("PLC:Legionowo");
    EXPECT_TRUE(src);
    shared_ptr<oh5::SourceRadar> radar =
        dynamic_pointer_cast<oh5::SourceRadar>(src);
    ASSERT_TRUE(radar);
    EXPECT_EQ("Legionowo", radar->place());
    EXPECT_EQ("PL41", radar->radar_site());
    ASSERT_TRUE(radar->centre());
    EXPECT_EQ("pl", radar->centre()->node_id());
}

TEST_P(rdb_RelationalDatabase_test, load_source_by_plc_unicode) {
    shared_ptr<oh5::Source> src = db->load_source(String::from_utf8("PLC:Świdwin"));
    EXPECT_TRUE(src);
    shared_ptr<oh5::SourceRadar> radar =
        dynamic_pointer_cast<oh5::SourceRadar>(src);
    EXPECT_TRUE(src);
    EXPECT_EQ(String::from_utf8("Świdwin"), radar->place());
    EXPECT_EQ("PL46", radar->radar_site());
    ASSERT_TRUE(radar->centre());
    EXPECT_EQ("pl", radar->centre()->node_id());
}

TEST_P(rdb_RelationalDatabase_test, load_same_source) {
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

TEST_P(rdb_RelationalDatabase_test, load_radars_with_same_centre) {
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

TEST_P(rdb_RelationalDatabase_test, save_file_with_invalid_attributes) {
    shared_ptr<oh5::File> file =
        oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    shared_ptr<oh5::Source> src = db->load_source(file->what_source());
    file->source(src);
    test::TempH5File tf;
    tf.write(*file);
    file->path(tf.path());
    // add an invalid attribute
    file->root()->add_child(make_shared<oh5::Attribute>("invalid"));

    EXPECT_NO_THROW(db->save_file(*file));
}

TEST_P(rdb_RelationalDatabase_test, attribute_groups_not_saved) {
    shared_ptr<oh5::File> file =
        oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    shared_ptr<oh5::Source> src = db->load_source(file->what_source());
    file->source(src);
    test::TempH5File tf;
    tf.write(*file);
    file->path(tf.path());
    
    ASSERT_NO_THROW(db->save_file(*file));
    
    /* XXX: figure out how to test this
    EXPECT_EQ(0, file->root()->child_group_by_name("what")->db_id());
    EXPECT_NE(0, file->root()->db_id());
    */
}

TEST_P(rdb_RelationalDatabase_test, resultset_keeps_qsqldatabase_alive) {
    shared_ptr<sql::Result> r;
    {
        RelationalDatabase db(GetParam());
        r = db.connection().execute("SELECT 1", sql::BindMap());
    };
    r->size();
}


#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RelationalDatabase_test_p,
                        rdb_RelationalDatabase_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace brfc
