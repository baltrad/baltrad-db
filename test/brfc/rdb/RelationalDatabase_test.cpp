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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/test/TestRDB.hpp>

#include "config.hpp"
#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_RelationalDatabase_test : public testing::TestWithParam<const char*> {
  public:
    rdb_RelationalDatabase_test()
            : db(TestRDBEnv::get_database(GetParam())) {

    }

    virtual void SetUp() {

    }

    virtual void TearDown() {
        db->clean();
    }

    test::TestRDB* db;
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
    shared_ptr<oh5::Source> src = db->load_source(QString::fromUtf8("PLC:Świdwin"));
    EXPECT_TRUE(src);
    shared_ptr<oh5::SourceRadar> radar =
        dynamic_pointer_cast<oh5::SourceRadar>(src);
    EXPECT_TRUE(src);
    EXPECT_EQ(QString::fromUtf8("Świdwin"), radar->place());
    EXPECT_EQ("PL46", radar->radar_site());
    ASSERT_TRUE(radar->centre());
    EXPECT_EQ("pl", radar->centre()->node_id());
}

TEST_P(rdb_RelationalDatabase_test, save_file_with_invalid_attributes) {
    shared_ptr<oh5::File> file =
        oh5::File::minimal("PVOL", QDate(2000, 1, 1), QTime(12, 0), "PLC:Legionowo");
    shared_ptr<oh5::Source> src = db->load_source(file->what_source());
    file->source(src);
    file->path("/path");
    file->root()->add_child(make_shared<oh5::Attribute>("invalid"));

    EXPECT_NO_THROW(db->save_file(*file, "test", 0));
}

TEST_P(rdb_RelationalDatabase_test, attribute_groups_not_saved) {
    shared_ptr<oh5::File> file =
        oh5::File::minimal("PVOL", QDate(2000, 1, 1), QTime(12, 0), "PLC:Legionowo");
    shared_ptr<oh5::Source> src = db->load_source(file->what_source());
    file->source(src);
    file->path("/path");
    
    ASSERT_NO_THROW(db->save_file(*file, "test", 0));
    
    EXPECT_EQ(0, file->root()->child_group_by_name("what")->db_id());
    EXPECT_NE(0, file->root()->db_id());
}


#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RelationalDatabase_test_p,
                        rdb_RelationalDatabase_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace brfc
