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
#include <brfc/FileHasher.hpp>
#include <brfc/StringList.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/AttributeResult.hpp>
#include <brfc/db/FileEntry.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include "config.hpp"
#include "../../common.hpp"

namespace brfc {
namespace db {
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
struct db_rdb_AttributeQuery_test : public testing::TestWithParam<const char*> {
    db_rdb_AttributeQuery_test()
            : xpr()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(TestRDBEnv::get_database(GetParam()))
            , td1("PVOL", Date(2000, 1, 1), Time(12, 0), src1)
            , td2("PVOL", Date(2000, 1, 1), Time(12, 1), src2)
            , td3("PVOL", Date(2000, 1, 1), Time(12, 2), src1)
            , td4("CVOL", Date(2001, 1, 1), Time(12, 0), src2)
            , td5("SCAN", Date(2002, 2, 1), Time(12, 0), src1)
            , query(db) {
    }

    void add_attribute(oh5::File& file, const String& path, const oh5::Scalar& value) {
        StringList names = path.split("/");
        String attr_name = names.take_last();

        oh5::Group& g = file.root().get_or_create_group(names.join("/"));
        g.create_attribute(attr_name, value);

    }

    virtual void SetUp() {
        add_attribute(td1, "dataset1/where/xsize", oh5::Scalar(1));
        add_attribute(td1, "dataset1/where/ysize", oh5::Scalar(2));
        tf1.write(td1);
        td1.path(tf1.path());

        add_attribute(td2, "dataset1/where/xsize", oh5::Scalar(2));
        add_attribute(td2, "dataset1/where/ysize", oh5::Scalar(2));
        tf2.write(td2);
        td2.path(tf2.path());

        add_attribute(td3, "dataset1/where/xsize", oh5::Scalar(3));
        add_attribute(td3, "dataset2/where/xsize", oh5::Scalar(3));
        tf3.write(td3);
        td3.path(tf3.path());

        add_attribute(td4, "dataset1/where/xsize", oh5::Scalar(6));
        add_attribute(td4, "dataset1/where/ysize", oh5::Scalar(4));
        add_attribute(td4, "dataset2/where/ysize", oh5::Scalar(5));
        tf4.write(td4);
        td4.path(tf4.path());

        add_attribute(td5, "dataset1/where/xsize", oh5::Scalar(5));
        add_attribute(td5, "dataset1/where/ysize", oh5::Scalar(2));
        add_attribute(td5, "dataset2/where/xsize", oh5::Scalar(2));
        add_attribute(td5, "dataset2/where/ysize", oh5::Scalar(5));
        tf5.write(td5);
        td5.path(tf5.path());

        fe1 = db->store(td1);
        fe2 = db->store(td2);
        fe3 = db->store(td3);
        fe4 = db->store(td4);
        fe5 = db->store(td5);
    }

    virtual void TearDown() {
        db->clean();
    }
    
    expr::ExpressionFactory xpr;
    String src1, src2;
    test::TestRDB* db;
    oh5::hl::HlFile td1, td2, td3, td4, td5;
    test::TempH5File tf1, tf2, tf3, tf4, tf5;
    shared_ptr<FileEntry> fe1, fe2, fe3, fe4, fe5;
    AttributeQuery query;
};


TEST_P(db_rdb_AttributeQuery_test, test_simple) {
    shared_ptr<AttributeResult> r = 
            query.fetch(*xpr.attribute("file:uuid"))
                 .filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(1)))
                 .execute();
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(not r->next());
}

TEST_P(db_rdb_AttributeQuery_test, test_list_all_files) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .execute();

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_filter_by_object) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*xpr.attribute("what/object")->eq(*xpr.string("PVOL")))
             .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC)
             .execute();

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_fetch_xsize_filtering_by_xsize) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("where/xsize"))
             .filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(2)))
             .execute();

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(2, r->int64_(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(2, r->int64_(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*xsize->eq(*xpr.int64_(1))->or_(*ysize->eq(*xpr.int64_(2))))
             .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC)
             .execute();

    EXPECT_EQ(4, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0)); // xsize matches
    ASSERT_TRUE(r->next()); 
    EXPECT_EQ(fe1->uuid(), r->string(0)); // ysize matches
    ASSERT_TRUE(r->next()); 
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_filter_by_xsize_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<AttributeResult> r = 
        query.fetch(*xpr.attribute("file:uuid"))
             .distinct(true)
             .filter(*xsize->eq(*xpr.int64_(3)))
             .execute();
    EXPECT_EQ(1, r->size());

    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_filter_by_combined_datetime) {
    DateTime min(2000, 1, 1, 12, 1);
    DateTime max(2001, 1, 1, 12, 0);
    expr::ExpressionPtr what_dt =
        xpr.add(
            *xpr.attribute("what/date"),
            *xpr.attribute("what/time")
        )->parentheses();

    query.fetch(*xpr.attribute("file:uuid"));
    query.filter(*what_dt->between(*xpr.datetime(min), *xpr.datetime(max)));
    shared_ptr<AttributeResult> r = query.execute();

    EXPECT_EQ(r->size(), 3);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("what/source:WMO");
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*wmo_code->eq(*xpr.string("02666")))
             .execute();

    EXPECT_EQ(r->size(), 2);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_select_by_or_node) {
    expr::AttributePtr node = xpr.attribute("what/source:node");
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*node->eq(*xpr.string("seang"))->or_(*node->eq(*xpr.string("sekkr"))))
             .execute();
    EXPECT_EQ(r->size(), 5);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_select_by_node_and_node) {
    expr::AttributePtr node = xpr.attribute("what/source:node");
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*node->eq(*xpr.string("seang"))->and_(*node->eq(*xpr.string("sekkr"))))
             .execute();
    EXPECT_EQ(0, r->size());
}

TEST_P(db_rdb_AttributeQuery_test, test_select_by_place) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*xpr.attribute("what/source:PLC")->eq(*xpr.string("Ängelholm")))
             .execute();

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
}

// temporarily disabled, see Query::fetch() for details
TEST_P(db_rdb_AttributeQuery_test, DISABLED_test_duplicate_fetch_throws) {
    query.fetch(*xpr.attribute("file:uuid"));
    EXPECT_THROW(query.fetch(*xpr.attribute("file:uuid")), duplicate_entry);
}

TEST_P(db_rdb_AttributeQuery_test, test_query_like) {
    expr::AttributePtr node = xpr.attribute("what/source:node");
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .filter(*node->like("sea*"))
             .execute();

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_order_by) {
    expr::ExpressionPtr dt =
        xpr.attribute("what/date")->add(*xpr.attribute("what/time"));
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .order_by(*dt, AttributeQuery::DESC)
             .execute();

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_limit) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.attribute("file:uuid"))
             .limit(2)
             .execute();
    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string(0));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string(0));
}

TEST_P(db_rdb_AttributeQuery_test, test_fetch_max_xsize) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.max(*xpr.attribute("where/xsize")))
             .execute();
    
    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(6, r->value_at(0).int64_());
}

TEST_P(db_rdb_AttributeQuery_test, test_fetch_min_ysize_with_filter) {
    shared_ptr<AttributeResult> r =
        query.fetch(*xpr.min(*xpr.attribute("where/ysize")))
             .filter(*xpr.attribute("what/object")->eq(*xpr.string("CVOL")))
             .execute();
    
    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(4, r->value_at(0).int64_());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_rdb_AttributeQuery_test_p,
                        db_rdb_AttributeQuery_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace rdb
} // namespace db
} // namespace brfc

