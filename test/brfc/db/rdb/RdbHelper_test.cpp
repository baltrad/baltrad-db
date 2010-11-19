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

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include <brfc/Date.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../../common.hpp"

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RdbHelper_test : public ::testing::TestWithParam<const char*> {
  public:
    db_rdb_RdbHelper_test()
            : db(TestRDBEnv::get_database(GetParam()))
            , conn(db->conn())
            , helper(conn)
            , entry(db)
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
    shared_ptr<sql::Connection> conn;
    RdbHelper helper;
    RdbFileEntry entry;
    oh5::hl::HlFile file;
};

TEST_P(db_rdb_RdbHelper_test, test_backend) {
    oh5::Group grp("grp");
    EXPECT_THROW(helper.backend(grp), std::runtime_error);

    grp.backend(new RdbNodeBackend());
    EXPECT_NO_THROW(helper.backend(grp));
}

TEST_P(db_rdb_RdbHelper_test, test_insert_file) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    EXPECT_GT(entry.id(), 0);
    EXPECT_TRUE("" != entry.uuid());
    EXPECT_GT(entry.source_id(), 0);
    EXPECT_EQ("hash", entry.hash());
    EXPECT_NE(DateTime(), entry.stored_at());
    EXPECT_GT(static_cast<RdbFileEntry*>(entry.root().file())->id(), 0);
}

TEST_P(db_rdb_RdbHelper_test, test_insert_file_content) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    test::TempH5File tf;
    tf.write(file);
 
    EXPECT_NO_THROW(helper.insert_file_content(entry, tf.path()));
    EXPECT_GT(entry.lo_id(), 0);

    EXPECT_NO_THROW(conn->commit());
}

TEST_P(db_rdb_RdbHelper_test, test_insert_node_group) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));

    oh5::Group& grp = entry.root();
    EXPECT_NO_THROW(helper.insert_node(grp));
    EXPECT_GT(helper.backend(grp).id(), 0);
    EXPECT_TRUE(helper.backend(grp).loaded());

    EXPECT_ANY_THROW(helper.insert_node(file.root()));
}

TEST_P(db_rdb_RdbHelper_test, test_insert_node_attribute) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.root()));
    
    oh5::Attribute attr("attr", oh5::Scalar(1));
    attr.backend(new RdbNodeBackend());
    attr.parent(&entry.root());

    EXPECT_NO_THROW(helper.insert_node(attr));
}

TEST_P(db_rdb_RdbHelper_test, test_insert_node_dataset) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.root()));
    oh5::Node* parent = 0;
    EXPECT_NO_THROW(parent = &entry.root().create_group("data1"));
    EXPECT_NO_THROW(helper.insert_node(*parent));
    
    oh5::DataSet ds("data");
    ds.backend(new RdbNodeBackend());
    ds.parent(parent);

    helper.insert_node(ds);
    EXPECT_NO_THROW(helper.insert_node(ds));
    EXPECT_GT(helper.backend(ds).id(), 0);
}

TEST_P(db_rdb_RdbHelper_test, test_load_file_by_id) {
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
    EXPECT_EQ(entry.stored_at(), e2.stored_at());
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(entry.lo_id(), e2.lo_id());
}

TEST_P(db_rdb_RdbHelper_test, test_load_file_by_uuid) {
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
    EXPECT_EQ(entry.stored_at(), e2.stored_at());
    EXPECT_GT(e2.lo_id(), 0);
    EXPECT_EQ(entry.lo_id(), e2.lo_id());
}

TEST_P(db_rdb_RdbHelper_test, test_select_root_id_by_id) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.root()));

    RdbFileEntry e2(db);
    e2.id(entry.id());

    long long id = 0;
    EXPECT_NO_THROW(id = helper.select_root_id(e2));
    EXPECT_EQ(helper.backend(entry.root()).id(), id);
}

TEST_P(db_rdb_RdbHelper_test, test_select_root_id_by_uuid) {
    EXPECT_NO_THROW(helper.insert_file(entry, file));
    EXPECT_NO_THROW(helper.insert_node(entry.root()));
    conn->commit();

    RdbFileEntry e2(db);
    e2.uuid(entry.uuid());
    
    long long id = 0;
    id = helper.select_root_id(e2);
    EXPECT_NO_THROW(id = helper.select_root_id(e2));
    EXPECT_EQ(helper.backend(entry.root()).id(), id);
}


TEST_P(db_rdb_RdbHelper_test, test_load_source) {
    long long src_id = 0;
    oh5::Source src = oh5::Source::from_string("PLC:Legionowo");
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);
    
    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.get("RAD"));
    ASSERT_TRUE(src.has("name"));
    EXPECT_EQ("plleg", src.get("name"));
}

TEST_P(db_rdb_RdbHelper_test, test_select_source_nx) {
    oh5::Source src;
    EXPECT_NO_THROW(helper.select_source(-1));
    EXPECT_TRUE(src.empty());
}

TEST_P(db_rdb_RdbHelper_test, test_load_source_unicode) {
    long long src_id = 0;
    oh5::Source src = oh5::Source::from_string("PLC:Legionowo");
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);
    
    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ("Legionowo", src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL41", src.get("RAD"));
    ASSERT_TRUE(src.has("name"));
    EXPECT_EQ("plleg", src.get("name"));
}

TEST_P(db_rdb_RdbHelper_test, test_load_source_by_plc_unicode) {
    long long src_id = 0;
    oh5::Source src = oh5::Source::from_string(String::from_utf8("PLC:Świdwin"));
    EXPECT_NO_THROW(src_id = helper.select_source_id(src));
    EXPECT_GT(src_id, 0);

    src.clear();
    EXPECT_NO_THROW(src = helper.select_source(src_id));

    ASSERT_TRUE(src.has("PLC"));
    EXPECT_EQ(String::from_utf8("Świdwin"), src.get("PLC"));
    ASSERT_TRUE(src.has("RAD"));
    EXPECT_EQ("PL46", src.get("RAD"));
    ASSERT_TRUE(src.has("name"));
    EXPECT_EQ("plswi", src.get("name"));
}

TEST_P(db_rdb_RdbHelper_test, test_load_children) {

    oh5::Group *g1, *g2;
    oh5::Attribute *a1, *a2, *a3; 
    oh5::DataSet* ds1;

    g1 = &entry.root().create_group("g1");
    g2 = &entry.root().create_group("g2");
    a1 = &entry.root().create_attribute("a1", oh5::Scalar(1));
    a2 = &g2->create_attribute("a2", oh5::Scalar(2));
    ds1 = &g2->create_dataset("ds1");
    a3 = &ds1->create_attribute("a3", oh5::Scalar(3));

    EXPECT_NO_THROW(helper.insert_file(entry, file));
    BOOST_FOREACH(oh5::Node& node, entry.root()) {
        EXPECT_NO_THROW(helper.insert_node(node));
    }
    conn->commit();
    
    oh5::RootGroup r(&entry);
    r.backend(new RdbNodeBackend());
    helper.backend(r).id(helper.backend(entry.root()).id());
    helper.backend(r).loaded(false);

    EXPECT_NO_THROW(helper.load_children(r));
    EXPECT_TRUE(helper.backend(r).loaded());

    EXPECT_EQ(3, r.children().size());
    
    oh5::Group* g = r.group("g1");
    ASSERT_TRUE(g);
    EXPECT_EQ(helper.backend(*g1).id(), helper.backend(*g).id());
    EXPECT_FALSE(helper.backend(*g).loaded());
    
    g = r.group("g2");
    ASSERT_TRUE(g);
    EXPECT_EQ(helper.backend(*g2).id(), helper.backend(*g).id());
    EXPECT_FALSE(helper.backend(*g).loaded());

    oh5::Attribute* a = r.attribute("a1");
    ASSERT_TRUE(a);
    EXPECT_EQ(helper.backend(*a1).id(), helper.backend(*a).id());
    EXPECT_FALSE(helper.backend(*a).loaded());
    EXPECT_EQ(a1->value(), a->value());

    oh5::DataSet* d = dynamic_cast<oh5::DataSet*>(r.child("g2/ds1"));
    ASSERT_TRUE(d);
    EXPECT_EQ(helper.backend(*ds1).id(), helper.backend(*d).id());
    EXPECT_FALSE(helper.backend(*d).loaded());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_rdb_RdbHelper_test_p,
                        db_rdb_RdbHelper_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace db
} // namespace brfc
