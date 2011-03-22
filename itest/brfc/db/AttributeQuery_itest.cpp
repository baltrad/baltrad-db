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

#include <boost/foreach.hpp>
#include <boost/algorithm/string/erase.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/AttributeResult.hpp>
#include <brfc/db/FileEntry.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/ExpressionList.hpp>
#include <brfc/expr/Function.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

namespace brfc {
namespace db {

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
struct db_AttributeQuery_itest : public testing::TestWithParam<const char*> {
    db_AttributeQuery_itest()
            : xpr()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(ITestEnv::get_database(GetParam()))
            , td1("PVOL", Date(2000, 1, 1), Time(12, 0), src1)
            , td2("PVOL", Date(2000, 1, 1), Time(12, 1), src2)
            , td3("PVOL", Date(2000, 1, 1), Time(12, 2), src1)
            , td4("CVOL", Date(2001, 1, 1), Time(12, 0), src2)
            , td5("SCAN", Date(2002, 2, 1), Time(12, 0), src1)
            , query()
            , r() {
    }

    void add_attribute(oh5::File& file,
                       std::string path,
                       const oh5::Scalar& value) {
        const std::string attr_name = path.substr(path.rfind('/') + 1);
        boost::erase_tail(path, attr_name.length() + 1);

        oh5::Node& g = static_cast<oh5::Group&>(file.root()).get_or_create_group(path);
        g.add(new oh5::Attribute(attr_name, value));
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

        fe1.reset(db->store(td1));
        fe2.reset(db->store(td2));
        fe3.reset(db->store(td3));
        fe4.reset(db->store(td4));
        fe5.reset(db->store(td5));
    }

    virtual void TearDown() {
        db->clean();
    }
    
    expr::ExpressionFactory xpr;
    std::string src1, src2;
    test::TestRDB* db;
    oh5::hl::HlFile td1, td2, td3, td4, td5;
    test::TempH5File tf1, tf2, tf3, tf4, tf5;
    scoped_ptr<FileEntry> fe1, fe2, fe3, fe4, fe5;
    AttributeQuery query;
    scoped_ptr<AttributeResult> r;
};


TEST_P(db_AttributeQuery_itest, test_simple) {
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(1)));
    r.reset(db->execute(query));

    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(not r->next());
}

TEST_P(db_AttributeQuery_itest, test_list_all_files) {
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_filter_by_object) {
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xpr.attribute("what/object")->eq(*xpr.string("PVOL")))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_fetch_xsize_filtering_by_xsize) {
    query.fetch("xsize", *xpr.attribute("where/xsize"))
         .filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(2)))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(2, r->int64_("xsize"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(2, r->int64_("xsize"));
}

TEST_P(db_AttributeQuery_itest, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xsize->eq(*xpr.int64_(1))->or_(*ysize->eq(*xpr.int64_(2))))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(4, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid")); // xsize & ysize matches
    ASSERT_TRUE(r->next()); 
    EXPECT_EQ(fe2->uuid(), r->string("uuid")); // ysize matches
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid")); // ysize matches
    ASSERT_TRUE(r->next()); 
    EXPECT_EQ(fe5->uuid(), r->string("uuid")); // xsize matches
}

TEST_P(db_AttributeQuery_itest, test_filter_by_xsize_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .distinct(true)
         .filter(*xsize->eq(*xpr.int64_(3)));
    r.reset(db->execute(query));

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_filter_by_combined_datetime) {
    DateTime min(2000, 1, 1, 12, 1);
    DateTime max(2001, 1, 1, 12, 0);
    expr::ExpressionPtr what_dt =
        xpr.add(
            *xpr.attribute("what/date"),
            *xpr.attribute("what/time")
        )->parentheses();

    query.fetch("uuid", *xpr.attribute("file:uuid"));
    query.filter(*what_dt->between(*xpr.datetime(min), *xpr.datetime(max)));
    query.order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(r->size(), 3);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("what/source:WMO");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*wmo_code->eq(*xpr.string("02666")))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(r->size(), 2);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_select_by_or_node) {
    expr::AttributePtr node = xpr.attribute("what/source:_name");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*node->eq(*xpr.string("seang"))->or_(*node->eq(*xpr.string("sekkr"))))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));
    EXPECT_EQ(r->size(), 5);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_select_by_node_and_node) {
    expr::AttributePtr node = xpr.attribute("what/source:_name");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*node->eq(*xpr.string("seang"))->and_(*node->eq(*xpr.string("sekkr"))));
    r.reset(db->execute(query));
    EXPECT_EQ(0, r->size());
}

TEST_P(db_AttributeQuery_itest, test_select_by_place) {
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xpr.attribute("what/source:PLC")->eq(*xpr.string("Ängelholm")))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_query_like) {
    expr::AttributePtr node = xpr.attribute("what/source:_name");
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*node->like("sea*"))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_order_by) {
    expr::ExpressionPtr dt =
        xpr.attribute("what/date")->add(*xpr.attribute("what/time"));
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .order_by(*dt, AttributeQuery::DESC);
    r.reset(db->execute(query));

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_limit) {
    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC)
         .limit(2);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_fetch_max_xsize) {
    query.fetch("max_xsize", *xpr.max(*xpr.attribute("where/xsize")));
    r.reset(db->execute(query));
    
    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(6, r->value_at("max_xsize").int64_());
}

TEST_P(db_AttributeQuery_itest, test_count_ysize) {
    query.fetch("count_ysize", *xpr.count(*xpr.attribute("where/ysize")));
    r.reset(db->execute(query));

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(6, r->value_at("count_ysize").int64_());
}

TEST_P(db_AttributeQuery_itest, test_fetch_min_ysize_with_filter) {
    query.fetch("min_ysize", *xpr.min(*xpr.attribute("where/ysize")))
         .filter(*xpr.attribute("what/object")->eq(*xpr.string("CVOL")));
    r.reset(db->execute(query));
    
    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(4, r->value_at("min_ysize").int64_());
}

TEST_P(db_AttributeQuery_itest, test_group_by_source_sum_xsize) {
    query.fetch("source", *xpr.attribute("what/source:_name"))
         .fetch("sum_xsize", *xpr.sum(*xpr.attribute("where/xsize")))
         .group(*xpr.attribute("what/source:_name"))
         .order_by(*xpr.attribute("what/source:_name"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("seang", r->value_at("source").string());
    EXPECT_EQ(14, r->value_at("sum_xsize").to_int64());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("sekkr", r->value_at("source").string());
    EXPECT_EQ(8, r->value_at("sum_xsize").to_int64());
}

TEST_P(db_AttributeQuery_itest, test_group_by_source_sum_xsize_with_filter) {
    query.fetch("source", *xpr.attribute("what/source:_name"))
         .fetch("sum_xsize", *xpr.sum(*xpr.attribute("where/xsize")))
         .filter(*xpr.attribute("what/date")->eq(*xpr.date(2000, 1, 1)))
         .group(*xpr.attribute("what/source:_name"))
         .order_by(*xpr.attribute("what/source:_name"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("seang", r->value_at("source").string());
    EXPECT_EQ(7, r->value_at("sum_xsize").to_int64());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("sekkr", r->value_at("source").string());
    EXPECT_EQ(2, r->value_at("sum_xsize").to_int64());
}


TEST_P(db_AttributeQuery_itest, test_group_by_source_max_xsize) {
    query.fetch("source", *xpr.attribute("what/source:_name"))
         .fetch("max_xsize", *xpr.max(*xpr.attribute("where/xsize")))
         .group(*xpr.attribute("what/source:_name"))
         .order_by(*xpr.attribute("what/source:_name"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("seang", r->value_at("source").string());
    EXPECT_EQ(5, r->value_at("max_xsize").int64_());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("sekkr", r->value_at("source").string());
    EXPECT_EQ(6, r->value_at("max_xsize").int64_());
}

TEST_P(db_AttributeQuery_itest, test_group_by_source_max_xsize_min_ysize) {
    query.fetch("source", *xpr.attribute("what/source:_name"))
         .fetch("max_xsize", *xpr.max(*xpr.attribute("where/xsize")))
         .fetch("min_ysize", *xpr.min(*xpr.attribute("where/ysize")))
         .group(*xpr.attribute("what/source:_name"))
         .order_by(*xpr.attribute("what/source:_name"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("seang", r->value_at("source").string());
    EXPECT_EQ(5, r->value_at("max_xsize").int64_());
    EXPECT_EQ(2, r->value_at("min_ysize").int64_());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("sekkr", r->value_at("source").string());
    EXPECT_EQ(6, r->value_at("max_xsize").int64_());
    EXPECT_EQ(2, r->value_at("min_ysize").int64_());
}

TEST_P(db_AttributeQuery_itest, test_group_by_source_missing_value) {
    query.fetch("source", *xpr.attribute("what/source:_name"))
         .fetch("min_elangle", *xpr.min(*xpr.attribute("where/elangle")))
         .group(*xpr.attribute("what/source:_name"))
         .order_by(*xpr.attribute("what/source:_name"), AttributeQuery::ASC);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ("seang", r->value_at("source").string());
    EXPECT_TRUE(r->is_null("min_elangle"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ("sekkr", r->value_at("source").string());
    EXPECT_TRUE(r->is_null("min_elangle"));
}

TEST_P(db_AttributeQuery_itest, test_in) {
    expr::ExpressionList l;
    l.append(*xpr.string("CVOL"));
    l.append(*xpr.string("SCAN"));

    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xpr.attribute("what/object")->in(l))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));
    
    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

TEST_P(db_AttributeQuery_itest, test_not_in) {
    expr::ExpressionList l;
    l.append(*xpr.int64_(2));
    l.append(*xpr.int64_(3));

    query.fetch("uuid", *xpr.attribute("file:uuid"))
         .filter(*xpr.attribute("where/xsize")->not_in(l))
         .order_by(*xpr.attribute("file:stored_at"), AttributeQuery::ASC);
    r.reset(db->execute(query));
    
    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), r->string("uuid"));
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), r->string("uuid"));
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_AttributeQuery_itest_p,
                        db_AttributeQuery_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace db
} // namespace brfc

