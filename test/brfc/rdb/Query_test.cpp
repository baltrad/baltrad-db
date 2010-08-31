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

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/FileEntry.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/StringList.hpp>

#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/File.hpp>

#include <brfc/test/TestRDB.hpp>

#include "config.hpp"
#include "../common.hpp"
#include "../MockHasher.hpp"

using testing::Ref;
using testing::Return;

namespace brfc {
namespace rdb {

/**
 * test data for this fixture:
 *
 * src name obj  date       time  xsize ysize
 *   1 td1  PVOL 2000-01-01 12:00     1     2
 *   2 td2  PVOL 2000-01-01 12:01     2     2
 *   1 td3  PVOL 2000-01-01 12:02     3
 *                                    3
 *   2 td4  CVOL 2001-01-01 12:00     6     4
 *                                          5
 *   1 td5  SCAN 2002-02-01 12:00     5     2
 *                                    2     5
 */
struct rdb_Query_test : public testing::TestWithParam<const char*> {
    rdb_Query_test()
            : xpr()
            , hasher()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(TestRDBEnv::get_database(GetParam()))
            , td1(oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 0), src1))
            , td2(oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 1), src2))
            , td3(oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 2), src1))
            , td4(oh5::File::minimal("CVOL", Date(2001, 1, 1), Time(12, 0), src2))
            , td5(oh5::File::minimal("SCAN", Date(2002, 2, 1), Time(12, 0), src1))
            , query(db) {
    }

    void add_attribute(oh5::File& file, const String& path, const Variant& value) {
        StringList names = path.split("/");

        String dsname = names.take_first();
        shared_ptr<oh5::DataSetGroup> ds = dynamic_pointer_cast<oh5::DataSetGroup>(file.root()->child_by_name(dsname));
        if (not ds) {
            ds = make_shared<oh5::DataSetGroup>(dsname);
            file.root()->add_child(ds);
        }

        String groupname = names.take_first();
        shared_ptr<oh5::AttributeGroup> group = dynamic_pointer_cast<oh5::AttributeGroup>(ds->child_by_name(groupname));
        if (not group) {
            group = make_shared<oh5::AttributeGroup>(groupname);
            ds->add_child(group);
        }

        shared_ptr<oh5::Attribute> attr = make_shared<oh5::Attribute>(names.take_last(), value);
        group->add_child(attr);
    }

    virtual void SetUp() {
        db->file_hasher(&hasher);
        ON_CALL(hasher, do_name()).WillByDefault(Return("mock"));

        add_attribute(*td1, "dataset1/where/xsize", Variant(1));
        add_attribute(*td1, "dataset1/where/ysize", Variant(2));
        td1->source(db->load_source(src1));
        td1->path("td1");
        ON_CALL(hasher, do_hash(Ref(*td1))).WillByDefault(Return("td1"));

        add_attribute(*td2, "dataset1/where/xsize", Variant(2));
        add_attribute(*td2, "dataset1/where/ysize", Variant(2));
        td2->source(db->load_source(src2));
        td2->path("td2");
        ON_CALL(hasher, do_hash(Ref(*td2))).WillByDefault(Return("td2"));

        add_attribute(*td3, "dataset1/where/xsize", Variant(3));
        add_attribute(*td3, "dataset2/where/xsize", Variant(3));
        td3->source(db->load_source(src1));
        td3->path("td3");
        ON_CALL(hasher, do_hash(Ref(*td3))).WillByDefault(Return("td3"));

        add_attribute(*td4, "dataset1/where/xsize", Variant(6));
        add_attribute(*td4, "dataset1/where/ysize", Variant(4));
        add_attribute(*td4, "dataset2/where/ysize", Variant(5));
        td4->source(db->load_source(src2));
        td4->path("td4");
        ON_CALL(hasher, do_hash(Ref(*td4))).WillByDefault(Return("td4"));

        add_attribute(*td5, "dataset1/where/xsize", Variant(5));
        add_attribute(*td5, "dataset1/where/ysize", Variant(2));
        add_attribute(*td5, "dataset2/where/xsize", Variant(2));
        add_attribute(*td5, "dataset2/where/ysize", Variant(5));
        td5->source(db->load_source(src1));
        td5->path("td5");
        ON_CALL(hasher, do_hash(Ref(*td5))).WillByDefault(Return("td5"));

        fe1 = db->save_file(*td1);
        fe2 = db->save_file(*td2);
        fe3 = db->save_file(*td3);
        fe4 = db->save_file(*td4);
        fe5 = db->save_file(*td5);
    }

    virtual void TearDown() {
        db->file_hasher(0);
        db->clean();
    }
    
    expr::Factory xpr;
    ::testing::NiceMock<MockHasher> hasher;
    String src1, src2;
    test::TestRDB* db;
    shared_ptr<oh5::File> td1, td2, td3, td4, td5;
    shared_ptr<FileEntry> fe1, fe2, fe3, fe4, fe5;
    Query query;
};

TEST_P(rdb_Query_test, test_simple) {
    shared_ptr<ResultSet> r = 
            query.filter(xpr.attribute("where/xsize")->eq(xpr.int64_(1)))
                 .execute();
    ASSERT_EQ(1, r->size());
    EXPECT_EQ(fe1->id(), r->get(0)->id());
}

TEST_P(rdb_Query_test, test_list_all_files) {
    shared_ptr<ResultSet> r = query.execute();

    ASSERT_EQ(5, r->size());

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe2->id(), r->get(1)->id());
    EXPECT_EQ(fe3->id(), r->get(2)->id());
    EXPECT_EQ(fe4->id(), r->get(3)->id());
    EXPECT_EQ(fe5->id(), r->get(4)->id());
}

TEST_P(rdb_Query_test, test_filter_by_object) {
    shared_ptr<ResultSet> r =
        query.filter(xpr.attribute("what/object")->eq(xpr.string("PVOL")))
             .execute();

    ASSERT_EQ(3, r->size());

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe2->id(), r->get(1)->id());
    EXPECT_EQ(fe3->id(), r->get(2)->id());
}

TEST_P(rdb_Query_test, test_filter_by_xsize) {
    shared_ptr<ResultSet> r =
        query.filter(xpr.attribute("where/xsize")->eq(xpr.int64_(2)))
             .execute();
    ASSERT_EQ(2, r->size());

    EXPECT_EQ(fe2->id(), r->get(0)->id());
    EXPECT_EQ(fe5->id(), r->get(1)->id());
}

TEST_P(rdb_Query_test, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    shared_ptr<ResultSet> r =
        query.filter(xsize->eq(xpr.int64_(1))->or_(ysize->eq(xpr.int64_(2))))
             .execute();

    ASSERT_EQ(r->size(), 3);

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe2->id(), r->get(1)->id());
    EXPECT_EQ(fe5->id(), r->get(2)->id());
}

TEST_P(rdb_Query_test, test_filter_by_combined_datetime) {
    DateTime min(2000, 1, 1, 12, 1);
    DateTime max(2001, 1, 1, 12, 0);
    expr::ExpressionPtr what_dt =
        xpr.add(
            xpr.attribute("what/date"),
            xpr.attribute("what/time")
        )->parentheses();

    query.filter(what_dt->between(xpr.datetime(min), xpr.datetime(max)));
    shared_ptr<ResultSet> r = query.execute();

    ASSERT_EQ(3, r->size());

    EXPECT_EQ(fe2->id(), r->get(0)->id());
    EXPECT_EQ(fe3->id(), r->get(1)->id());
    EXPECT_EQ(fe4->id(), r->get(2)->id());
}

TEST_P(rdb_Query_test, test_filter_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("what/source:WMO");
    shared_ptr<ResultSet> r =
        query.filter(wmo_code->eq(xpr.int64_(2666)))
             .execute();

    ASSERT_EQ(2, r->size());

    EXPECT_EQ(fe2->id(), r->get(0)->id());
    EXPECT_EQ(fe4->id(), r->get(1)->id());
}

TEST_P(rdb_Query_test, test_filter_by_node_or_node) {
    expr::AttributePtr node = xpr.attribute("what/source:node");
    shared_ptr<ResultSet> r =
        query.filter(node->eq(xpr.string("seang"))->or_(node->eq(xpr.string("sekkr"))))
             .execute();
    ASSERT_EQ(r->size(), 5);

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe2->id(), r->get(1)->id());
    EXPECT_EQ(fe3->id(), r->get(2)->id());
    EXPECT_EQ(fe4->id(), r->get(3)->id());
    EXPECT_EQ(fe5->id(), r->get(4)->id());
}

TEST_P(rdb_Query_test, test_filter_by_node_and_node) {
    expr::AttributePtr node = xpr.attribute("what/source:node");
    shared_ptr<ResultSet> r =
        query.filter(node->eq(xpr.string("seang"))->and_(node->eq(xpr.string("sekkr"))))
             .execute();
    EXPECT_EQ(0, r->size());
}

TEST_P(rdb_Query_test, test_filter_by_place) {
    shared_ptr<ResultSet> r =
        query.filter(xpr.attribute("what/source:PLC")->eq(xpr.string("Ängelholm")))
             .execute();

    ASSERT_EQ(3, r->size());

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe3->id(), r->get(1)->id());
    EXPECT_EQ(fe5->id(), r->get(2)->id());
}

TEST_P(rdb_Query_test, test_has_file) {
    bool result = false;
    ASSERT_NO_THROW(result = db->has_file(*td1));
    EXPECT_TRUE(result);
}

TEST_P(rdb_Query_test, test_has_nx_file) {
    bool result = false;
    shared_ptr<oh5::File> td = oh5::File::minimal("PVOL", Date(2000, 1, 10), Time(12, 0), src1);
    EXPECT_CALL(hasher, do_hash(Ref(*td))).WillOnce(Return("td"));
    td->source(db->load_source(src1));
    ASSERT_NO_THROW(result = db->has_file(*td));
    EXPECT_FALSE(result);
}

/*
TEST_P(rdb_Query_test, test_query_file_id) {
    shared_ptr<ResultSet> r = 
        query.fetch(xpr.attribute("file:id"))
             .filter(xpr.attribute("file:path")->eq(xpr.string("td1")))
             .execute();
    EXPECT_EQ(1, r->size());
    EXPECT_EQ(fe1->id(), r->get(1)->int64_(0), 0);
}
*/

/*
TEST_P(rdb_Query_test, test_duplicate_fetch_throws) {
    query.fetch(xpr.attribute("file:path"));
    EXPECT_THROW(query.fetch(xpr.attribute("file:path")), duplicate_entry);
}
*/

TEST_P(rdb_Query_test, test_query_like) {
    shared_ptr<ResultSet> r =
        query.filter(xpr.attribute("what/source:node")->like("sea*"))
             .execute();
    ASSERT_EQ(3, r->size());

    EXPECT_EQ(fe1->id(), r->get(0)->id());
    EXPECT_EQ(fe3->id(), r->get(1)->id());
    EXPECT_EQ(fe5->id(), r->get(2)->id());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_Query_test_p,
                        rdb_Query_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace brfc

