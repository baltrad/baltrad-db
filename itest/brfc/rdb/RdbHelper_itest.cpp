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

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

#include <brfc/Date.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Time.hpp>

#include <brfc/rdb/RdbHelper.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/Oh5Source.hpp>
#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class rdb_RdbHelper_itest : public ::testing::TestWithParam<const char*> {
  public:
    rdb_RdbHelper_itest()
            : db(ITestEnv::get_database(GetParam()))
            , conn(db->conn())
            , helper(conn)
            , entry(db)
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Karlskrona") {
    }

    virtual void SetUp() {
        conn->begin();
        entry.loaded(true);
        entry.uuid("057c5a4b-d29c-4ac7-9267-d3fe7ef54000");
        entry.source_id(1);
        entry.what_object("PVOL");
        entry.what_date(Date(2000, 1, 1));
        entry.what_time(Time(12, 0));
        entry.stored_at(DateTime(2011, 1, 1, 12, 13, 14));
        entry.hash("hash");
    }

    virtual void TearDown() {
        if (conn->in_transaction())
            conn->rollback();
        db->clean();
    }

    test::TestRDB* db;
    boost::shared_ptr<sql::Connection> conn;
    RdbHelper helper;
    RdbFileEntry entry;
    HlFile file;
};

TEST_P(rdb_RdbHelper_itest, test_insert_file) {
    long long entry_id = helper.insert_file(entry);

    EXPECT_GT(entry_id, 0);
}

TEST_P(rdb_RdbHelper_itest, test_insert_file_content) {
    long long entry_id = helper.insert_file(entry);

    test::TempH5File tf;
    tf.write(file);
 
    long long lo_id = helper.insert_file_content(entry_id, tf.path(), 1);
    EXPECT_GT(lo_id, 0);

    EXPECT_NO_THROW(conn->commit());
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_group) {
    long long entry_id = helper.insert_file(entry);
    
    Oh5Node& grp = entry.root();
    long long id = helper.insert_node(entry_id, 0, grp);
    EXPECT_GT(id, 0);
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_attribute) {
    long long entry_id = helper.insert_file(entry);
    long long pid = helper.insert_node(entry_id, 0, entry.root());
    
    Oh5Node& attr = entry.root().add(new Oh5Attribute("attr", Oh5Scalar(1)));

    EXPECT_NO_THROW(helper.insert_node(entry_id, pid, attr));
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_dataset) {
    long long entry_id = helper.insert_file(entry);
    long long id = helper.insert_node(entry_id, 0, entry.root());
    Oh5Node& parent = entry.root().add(new Oh5Group("data1"));
    id = helper.insert_node(entry_id, id, parent);
    
    Oh5Node& ds = parent.add(new Oh5DataSet("data"));

    id = helper.insert_node(entry_id, id, ds);
    EXPECT_GT(id, 0);
}

TEST_P(rdb_RdbHelper_itest, test_load_file_by_id) {
    long long entry_id = helper.insert_file(entry);
    test::TempH5File tf;
    tf.write(file);
    long long lo_id = helper.insert_file_content(entry_id, tf.path(), 1);
    conn->commit();

    RdbFileEntry e2(db);
    e2.id(entry_id);

    EXPECT_NO_THROW(helper.load_file(e2));

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

TEST_P(rdb_RdbHelper_itest, test_load_file_by_uuid) {
    long long entry_id = helper.insert_file(entry);
    test::TempH5File tf;
    tf.write(file);
    long long lo_id = helper.insert_file_content(entry_id, tf.path(), 1);
    conn->commit();

    RdbFileEntry e2(db);
    e2.uuid(entry.uuid());

    EXPECT_NO_THROW(helper.load_file(e2));

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

TEST_P(rdb_RdbHelper_itest, test_load_source) {
    long long src_id = 0;
    Oh5Source src = Oh5Source::from_string("PLC:Legionowo");
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);
    
    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.get("RAD"));
    ASSERT_TRUE(src.has("_name"));
    EXPECT_EQ("plleg", src.get("_name"));
}

TEST_P(rdb_RdbHelper_itest, test_select_source_nx) {
    Oh5Source src;
    EXPECT_NO_THROW(helper.select_source(-1));
    EXPECT_TRUE(src.empty());
}

TEST_P(rdb_RdbHelper_itest, test_load_source_unicode) {
    long long src_id = 0;
    Oh5Source src = Oh5Source::from_string("PLC:Legionowo");
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);
    
    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.get("RAD"));
    ASSERT_TRUE(src.has("_name"));
    EXPECT_EQ("plleg", src.get("_name"));
}

TEST_P(rdb_RdbHelper_itest, test_load_source_by_plc_unicode) {
    long long src_id = 0;
    Oh5Source src = Oh5Source::from_string("PLC:Świdwin");
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);

    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Świdwin", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL46", src.get("RAD"));
    ASSERT_TRUE(src.has("_name"));
    EXPECT_EQ("plswi", src.get("_name"));
}

TEST_P(rdb_RdbHelper_itest, test_load_nodes) {
    Oh5Node& g1 = entry.root().add(new Oh5Group("g1"));
    Oh5Node& g2 = entry.root().add(new Oh5Group("g2"));
    Oh5Node& a1 = entry.root().add(new Oh5Attribute("a1", Oh5Scalar(1)));
    Oh5Node& a2 = g2.add(new Oh5Attribute("a2", Oh5Scalar(2)));
    Oh5Node& ds1 = g2.add(new Oh5DataSet("ds1"));
    Oh5Node& a3 = ds1.add(new Oh5Attribute("a3", Oh5Scalar(3)));

    long long entry_id = helper.insert_file(entry);
    helper.insert_nodes(entry_id, entry.root());
    conn->commit();
    
    Oh5MemoryNodeBackend be;
    Oh5Node& r = be.root();
    helper.load_nodes(entry_id, r);

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
INSTANTIATE_TEST_CASE_P(rdb_RdbHelper_itest_p,
                        rdb_RdbHelper_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
