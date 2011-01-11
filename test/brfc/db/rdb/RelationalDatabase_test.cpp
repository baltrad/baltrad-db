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

#include <brfc/test_config.hpp>
#include <brfc/test_common.hpp>

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
    
    template<typename T>
    typename T::const_iterator
    source_by_name(const T& sources, const std::string& name) {
        typename T::const_iterator i = sources.begin();
        for ( ; i != sources.end(); ++i) {
            if (i->get("_name") == name)
                break;
        }
        return i;
    }

    test::TestRDB* db;
};

TEST_P(db_rdb_RelationalDatabase_test, store) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());

    
    scoped_ptr<FileEntry> e;
    EXPECT_NO_THROW(e.reset(db->store(file)));
    RdbFileEntry* re = dynamic_cast<RdbFileEntry*>(e.get());
    ASSERT_TRUE(re);
    EXPECT_TRUE(re->id() > 0);
    EXPECT_TRUE(re->lo_id() > 0);
    EXPECT_FALSE(e->source().empty());
    EXPECT_TRUE(e->source().has("_name"));
}

TEST_P(db_rdb_RelationalDatabase_test, entry_by_uuid) {
    EXPECT_THROW(db->entry_by_uuid("nxuuid"), lookup_error);

    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    scoped_ptr<FileEntry> e1, e2;
    ASSERT_NO_THROW(e1.reset(db->store(file)));
     
    ASSERT_NO_THROW(e2.reset(db->entry_by_uuid(e1->uuid())));

    EXPECT_EQ(e1->uuid(), e2->uuid());
    EXPECT_EQ(e1->hash(), e2->hash());
}

TEST_P(db_rdb_RelationalDatabase_test, entry_by_file) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());

    EXPECT_THROW(db->entry_by_file(file), lookup_error);
    
    scoped_ptr<FileEntry> e1, e2;
    ASSERT_NO_THROW(e1.reset(db->store(file)));
    
    ASSERT_NO_THROW(e2.reset(db->entry_by_file(file)));

    EXPECT_EQ(e1->uuid(), e2->uuid());
    EXPECT_EQ(e1->hash(), e2->hash());
}

TEST_P(db_rdb_RelationalDatabase_test, remove) {
    oh5::hl::HlFile file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo");
    test::TempH5File tf;
    tf.write(file);
    file.path(tf.path());
    
    scoped_ptr<FileEntry> e;
    EXPECT_NO_THROW(e.reset(db->store(file)));
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
    
    scoped_ptr<FileEntry> e;
    EXPECT_NO_THROW(e.reset(db->store(file)));
    ASSERT_TRUE(e);
    
    // test writing
    test::TempH5File tef;
    EXPECT_NO_THROW(e->write_to_file(tef.path()));
    EXPECT_EQ(fs::file_size(tef.path()), fs::file_size(tf.path()));
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

TEST_P(db_rdb_RelationalDatabase_test, test_sources) {
    std::vector<oh5::Source> sources;
    EXPECT_NO_THROW(sources = db->sources());

    EXPECT_TRUE(sources.size() > 0);
}

TEST_P(db_rdb_RelationalDatabase_test, test_add_source) {
    oh5::Source src;
    EXPECT_THROW(db->add_source(src), lookup_error);

    src.add("_name", "srcname1");

    EXPECT_NO_THROW(db->add_source(src));

    std::vector<oh5::Source> sources = db->sources();

    EXPECT_TRUE(source_by_name(sources, "srcname1") != sources.end());

    src = *source_by_name(sources, "srcname1");
    EXPECT_EQ(0, src.keys().size());

    EXPECT_THROW(db->add_source(src), db_error);
}

TEST_P(db_rdb_RelationalDatabase_test, test_update_source) {
    oh5::Source src;
    src.add("_name", "srcname2");
    src.add("key1", "value1");
    src.add("key2", "value2");
    
    EXPECT_NO_THROW(db->add_source(src));

    std::vector<oh5::Source> sources = db->sources();
    ASSERT_TRUE(source_by_name(sources, "srcname2") != sources.end());
    src = *source_by_name(sources, "srcname2");
    src.remove("_name");
    src.remove("key2");
    src.add("_name", "srcname3");
    src.add("key3", "value3");
    EXPECT_NO_THROW(db->update_source(src));

    sources = db->sources();

    ASSERT_TRUE(source_by_name(sources, "srcname2") == sources.end());
    ASSERT_TRUE(source_by_name(sources, "srcname3") != sources.end());
    src = *source_by_name(sources, "srcname3");
    ASSERT_TRUE(src.has("key1"));
    EXPECT_EQ("value1", src.get("key1"));
    ASSERT_TRUE(src.has("key3"));
    EXPECT_EQ("value3", src.get("key3"));
}

TEST_P(db_rdb_RelationalDatabase_test, test_remove_source) {
    oh5::Source src;
    src.add("_name", "srcname4");

    EXPECT_THROW(db->remove_source(src), lookup_error);

    db->add_source(src);
    std::vector<oh5::Source> sources = db->sources(); 
    ASSERT_TRUE(source_by_name(sources, "srcname4") != sources.end());
    src = *source_by_name(sources, "srcname4");

    EXPECT_NO_THROW(db->remove_source(src));

    sources = db->sources();
    EXPECT_TRUE(source_by_name(sources, "srcname4") == sources.end());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_rdb_RelationalDatabase_test_p,
                        db_rdb_RelationalDatabase_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace db
} // namespace brfc
