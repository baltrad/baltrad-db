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

#include <brfc/Date.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Time.hpp>

#include <brfc/rdb/RdbDefaultFileManager.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5MemoryNodeBackend.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/Oh5Source.hpp>
#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class rdb_RdbDefaultFileManager_itest : public ::testing::TestWithParam<const char*> {
  public:
    rdb_RdbDefaultFileManager_itest()
            : db(ITestEnv::get_database(GetParam()))
            , conn(db->conn())
            , manager(conn)
            , entry(db)
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Karlskrona") {
    }

    virtual void SetUp() {
        conn->begin();
        entry.loaded(true);
        entry.uuid("057c5a4b-d29c-4ac7-9267-d3fe7ef54000");
        entry.source_id(1);
        entry.metadata().what_object("PVOL");
        entry.metadata().what_date(Date(2000, 1, 1));
        entry.metadata().what_time(Time(12, 0));
        entry.stored_at(DateTime(2011, 1, 1, 12, 13, 14));
        entry.hash("hash");
        entry.size(2);
    }

    virtual void TearDown() {
        if (conn->in_transaction())
            conn->rollback();
        db->clean();
    }

    test::TestRDB* db;
    boost::shared_ptr<sql::Connection> conn;
    RdbDefaultFileManager manager;
    RdbFileEntry entry;
    Oh5File file;
};

TEST_P(rdb_RdbDefaultFileManager_itest, test_insert_file) {
    long long entry_id = manager.insert_file(entry);

    EXPECT_GT(entry_id, 0);
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_insert_file_content) {
    long long entry_id = manager.insert_file(entry);
    
    NamedTemporaryFile tf;
    Oh5HlFileWriter writer;
    writer.write(file, tf.path());
 
    long long lo_id = manager.insert_file_content(entry_id, tf.path());
    EXPECT_GT(lo_id, 0);

    EXPECT_NO_THROW(conn->commit());
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_uuid_by_source_and_hash) {
    manager.insert_file(entry);
    
    std::string uuid = manager.uuid_by_source_and_hash(1, "hashh");
    EXPECT_EQ("", uuid);
    uuid = manager.uuid_by_source_and_hash(1, "hash");
    EXPECT_EQ(entry.uuid(), uuid);
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_insert_node_group) {
    long long entry_id = manager.insert_file(entry);
    
    Oh5Node& grp = entry.metadata().root();
    long long id = manager.insert_node(entry_id, 0, grp);
    EXPECT_GT(id, 0);
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_insert_node_attribute) {
    long long entry_id = manager.insert_file(entry);
    Oh5Node& root = entry.metadata().root();
    long long pid = manager.insert_node(entry_id, 0, root);
    
    Oh5Node& attr = root.add(new Oh5Attribute("attr", Oh5Scalar(1)));

    EXPECT_NO_THROW(manager.insert_node(entry_id, pid, attr));
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_insert_node_dataset) {
    long long entry_id = manager.insert_file(entry);
    Oh5Node& root = entry.metadata().root();
    long long id = manager.insert_node(entry_id, 0, root);
    Oh5Node& parent = root.add(new Oh5Group("data1"));
    id = manager.insert_node(entry_id, id, parent);
    
    Oh5Node& ds = parent.add(new Oh5DataSet("data"));

    id = manager.insert_node(entry_id, id, ds);
    EXPECT_GT(id, 0);
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_load_file_by_id) {
    long long entry_id = manager.insert_file(entry);
    NamedTemporaryFile tf;
    Oh5HlFileWriter writer;
    writer.write(file, tf.path());
    long long lo_id = manager.insert_file_content(entry_id, tf.path());
    conn->commit();

    RdbFileEntry e2(db);
    e2.id(entry_id);

    EXPECT_NO_THROW(manager.load_file(e2));

    EXPECT_GT(e2.source_id(), 0);
    EXPECT_EQ(entry.source_id(), e2.source_id());
    EXPECT_EQ("hash", e2.hash());
    EXPECT_EQ(entry.uuid(), e2.uuid());
    EXPECT_NE(DateTime(), e2.stored_at());
    DateTime dt1 = entry.stored_at(), dt2 = e2.stored_at();
    EXPECT_EQ(dt1, dt2);
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(lo_id, e2.lo_id());
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_load_file_by_uuid) {
    long long entry_id = manager.insert_file(entry);
    NamedTemporaryFile tf;
    Oh5HlFileWriter writer;
    writer.write(file, tf.path());
    long long lo_id = manager.insert_file_content(entry_id, tf.path());
    conn->commit();

    RdbFileEntry e2(db);
    e2.uuid(entry.uuid());

    EXPECT_NO_THROW(manager.load_file(e2));

    EXPECT_GT(e2.source_id(), 0);
    EXPECT_EQ(entry.source_id(), e2.source_id());
    EXPECT_EQ("hash", e2.hash());
    EXPECT_EQ(entry.uuid(), e2.uuid());
    EXPECT_NE(DateTime(), e2.stored_at());
    DateTime dt1 = entry.stored_at(), dt2 = e2.stored_at();
    EXPECT_EQ(dt1, dt2);
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(lo_id, e2.lo_id());
}

TEST_P(rdb_RdbDefaultFileManager_itest, test_load_nodes) {
    Oh5Node& root = entry.metadata().root();
    Oh5Node& g1 = root.add(new Oh5Group("g1"));
    Oh5Node& g2 = root.add(new Oh5Group("g2"));
    Oh5Node& a1 = root.add(new Oh5Attribute("a1", Oh5Scalar(1)));
    Oh5Node& a2 = g2.add(new Oh5Attribute("a2", Oh5Scalar(2)));
    Oh5Node& ds1 = g2.add(new Oh5DataSet("ds1"));
    Oh5Node& a3 = ds1.add(new Oh5Attribute("a3", Oh5Scalar(3)));

    long long entry_id = manager.insert_file(entry);
    manager.insert_nodes(entry_id, root);
    conn->commit();
    
    Oh5MemoryNodeBackend be;
    Oh5Node& r = be.root();
    manager.load_nodes(entry_id, r);

    EXPECT_EQ(4u, r.children().size());

    Oh5Node* g = r.group("g1");
    ASSERT_TRUE(g);
    
    g = r.group("g2");
    ASSERT_TRUE(g);

    Oh5Attribute* a = r.attribute("a1");
    ASSERT_TRUE(a);
    EXPECT_EQ(Oh5Scalar(1), a->value());

    Oh5DataSet* d = dynamic_cast<Oh5DataSet*>(r.child("g2/ds1"));
    ASSERT_TRUE(d);
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RdbDefaultFileManager_itest_p,
                        rdb_RdbDefaultFileManager_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
