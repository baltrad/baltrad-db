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
#include <brfc/rdb/RdbOh5NodeBackend.hpp>

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
            , backend(entry.node_backend())
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Karlskrona") {
    }

    virtual void SetUp() {
        conn->begin();
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
    RdbOh5NodeBackend& backend;
    HlFile file;
};

TEST_P(rdb_RdbHelper_itest, test_backend) {
    Oh5Group* grp = new Oh5Group("grp");
    EXPECT_THROW(helper.backend(*grp), std::runtime_error);
    
    entry.root().add(grp);
    EXPECT_NO_THROW(helper.backend(*grp));
}

TEST_P(rdb_RdbHelper_itest, test_insert_file) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    EXPECT_GT(entry.id(), 0);
    EXPECT_TRUE("" != entry.uuid());
    EXPECT_GT(entry.source_id(), 0);
    EXPECT_EQ("hash", entry.hash());
    EXPECT_NE(DateTime(), entry.stored_at());
    EXPECT_GT(entry.id(), 0);
}

TEST_P(rdb_RdbHelper_itest, test_insert_file_content) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    test::TempH5File tf;
    tf.write(file);
 
    EXPECT_NO_THROW(helper.insert_file_content(entry, tf.path()));
    EXPECT_GT(entry.lo_id(), 0);
    EXPECT_GT(entry.size(), 0);

    EXPECT_NO_THROW(conn->commit());
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_group) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    Oh5Node& grp = entry.root();
    EXPECT_NO_THROW(helper.insert_node(entry.id(), grp));
    EXPECT_GT(backend.id(grp), 0);
    EXPECT_TRUE(backend.loaded(grp));

    EXPECT_ANY_THROW(helper.insert_node(entry.id(), file.root()));
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_attribute) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.id(), entry.root()));
    
    Oh5Node& attr = entry.root().add(new Oh5Attribute("attr", Oh5Scalar(1)));

    EXPECT_NO_THROW(helper.insert_node(entry.id(), attr));
}

TEST_P(rdb_RdbHelper_itest, test_insert_node_dataset) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.id(), entry.root()));
    Oh5Node& parent = entry.root().add(new Oh5Group("data1"));
    EXPECT_NO_THROW(helper.insert_node(entry.id(), parent));
    
    Oh5Node& ds = parent.add(new Oh5DataSet("data"));

    EXPECT_NO_THROW(helper.insert_node(entry.id(), ds));
    EXPECT_GT(backend.id(ds), 0);
}

TEST_P(rdb_RdbHelper_itest, test_load_file_by_id) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    test::TempH5File tf;
    tf.write(file);
    EXPECT_NO_THROW(helper.insert_file_content(entry, tf.path()));
    conn->commit();

    RdbFileEntry e2(db);
    e2.id(entry.id());

    EXPECT_NO_THROW(helper.load_file(e2));

    EXPECT_GT(e2.source_id(), 0);
    EXPECT_EQ(entry.source_id(), e2.source_id());
    EXPECT_EQ("hash", e2.hash());
    EXPECT_EQ(entry.uuid(), e2.uuid());
    EXPECT_NE(DateTime(), e2.stored_at());
    DateTime dt1 = entry.stored_at(), dt2 = e2.stored_at();
    dt1.time().msec(0);
    dt2.time().msec(0);
    EXPECT_EQ(dt1, dt2);
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(entry.lo_id(), e2.lo_id());
}

TEST_P(rdb_RdbHelper_itest, test_load_file_by_uuid) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    test::TempH5File tf;
    tf.write(file);
    EXPECT_NO_THROW(helper.insert_file_content(entry, tf.path()));
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
    dt1.time().msec(0);
    dt2.time().msec(0);
    EXPECT_EQ(dt1, dt2);
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(entry.lo_id(), e2.lo_id());
}

TEST_P(rdb_RdbHelper_itest, test_select_root_id_by_id) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.id(), entry.root()));

    RdbFileEntry e2(db);
    e2.id(entry.id());

    long long id = 0;
    EXPECT_NO_THROW(id = helper.select_root_id(e2));
    EXPECT_EQ(backend.id(entry.root()), id);
}

TEST_P(rdb_RdbHelper_itest, test_select_root_id_by_uuid) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.id(), entry.root()));
    conn->commit();

    RdbFileEntry e2(db);
    e2.uuid(entry.uuid());
    
    long long id = 0;
    id = helper.select_root_id(e2);
    EXPECT_NO_THROW(id = helper.select_root_id(e2));
    EXPECT_EQ(backend.id(entry.root()), id);
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

TEST_P(rdb_RdbHelper_itest, test_load_children) {
    Oh5Node& g1 = entry.root().add(new Oh5Group("g1"));
    Oh5Node& g2 = entry.root().add(new Oh5Group("g2"));
    Oh5Node& a1 = entry.root().add(new Oh5Attribute("a1", Oh5Scalar(1)));
    Oh5Node& a2 = g2.add(new Oh5Attribute("a2", Oh5Scalar(2)));
    Oh5Node& ds1 = g2.add(new Oh5DataSet("ds1"));
    Oh5Node& a3 = ds1.add(new Oh5Attribute("a3", Oh5Scalar(3)));

    EXPECT_NO_THROW(helper.insert_file(entry, file));
    BOOST_FOREACH(Oh5Node& node, entry.root()) {
        EXPECT_NO_THROW(helper.insert_node(entry.id(), node));
    }
    conn->commit();

    RdbOh5NodeBackend be(db);
    Oh5Node& r = be.root();
    be.id(r, backend.id(entry.root()));
    be.loaded(r, false);

    EXPECT_NO_THROW(helper.load_children(r));
    EXPECT_TRUE(be.loaded(r));

    EXPECT_EQ(3u, r.children().size());

    Oh5Node* g = r.group("g1");
    ASSERT_TRUE(g);
    EXPECT_EQ(backend.id(g1), be.id(*g));
    EXPECT_FALSE(be.loaded(*g));
    
    g = r.group("g2");
    ASSERT_TRUE(g);
    EXPECT_EQ(backend.id(g2), be.id(*g));
    EXPECT_FALSE(be.loaded(*g));

    Oh5Attribute* a = r.attribute("a1");
    ASSERT_TRUE(a);
    EXPECT_EQ(backend.id(a1), be.id(*a));
    EXPECT_FALSE(be.loaded(*a));
    EXPECT_EQ(Oh5Scalar(1), a->value());

    Oh5DataSet* d = dynamic_cast<Oh5DataSet*>(r.child("g2/ds1"));
    ASSERT_TRUE(d);
    EXPECT_EQ(backend.id(ds1), be.id(*d));
    EXPECT_FALSE(be.loaded(*d));
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_RdbHelper_itest_p,
                        rdb_RdbHelper_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
