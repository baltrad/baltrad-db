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
#include <boost/algorithm/string/erase.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>

#include <brfc/db/FileEntry.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/FileResult.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/oh5/Scalar.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

//#include <brfc/test_common.hpp>

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
struct db_FileQuery_itest : public testing::TestWithParam<const char*> {
    db_FileQuery_itest()
            : xpr()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(ITestEnv::get_database(GetParam()))
            , td1("PVOL", Date(2000, 1, 1), Time(12, 0), src1)
            , td2("PVOL", Date(2000, 1, 1), Time(12, 1), src2)
            , td3("PVOL", Date(2000, 1, 1), Time(12, 2), src1)
            , td4("CVOL", Date(2001, 1, 1), Time(12, 0), src2)
            , td5("SCAN", Date(2002, 2, 1), Time(12, 0), src1)
            , query(db) {
    }

    void add_attribute(oh5::File& file,
                       std::string path,
                       const oh5::Scalar& value) {
        const std::string attr_name = path.substr(path.rfind('/') + 1);
        boost::erase_tail(path, attr_name.length() + 1);

        oh5::Group& g = file.root().get_or_create_group(path);
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

        fe1.reset(db->store(td1));
        fe2.reset(db->store(td2));
        fe3.reset(db->store(td3));
        fe4.reset(db->store(td4));
        fe5.reset(db->store(td5));
    }

    virtual void TearDown() {
        db->clean();
    }

    auto_ptr<db::FileEntry> entry(FileResult& r) {
        return auto_ptr<db::FileEntry>(r.entry());
    }
    
    expr::ExpressionFactory xpr;
    std::string src1, src2;
    test::TestRDB* db;
    oh5::hl::HlFile td1, td2, td3, td4, td5;
    test::TempH5File tf1, tf2, tf3, tf4, tf5;
    scoped_ptr<FileEntry> fe1, fe2, fe3, fe4, fe5;
    FileQuery query;
    scoped_ptr<FileResult> r;
};

/*
TEST_P(db_FileQuery_itest, test_queried_entry_has_lob) {
    query.filter(*xpr.attribute("file:uuid")->eq(*xpr.string(fe1->uuid())));
    r.reset(query.execute());

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());

    auto_ptr<FileEntry> aqre(entry(*r));
    RdbFileEntry* qre = dynamic_cast<RdbFileEntry*>(aqre.get());
    RdbFileEntry* fre1 = dynamic_cast<RdbFileEntry*>(fe1.get());
    
    ASSERT_TRUE(qre);
    ASSERT_TRUE(fre1);
    
    EXPECT_TRUE(qre->lo_id() > 0);
    EXPECT_TRUE(qre->id() > 0);
    EXPECT_EQ(qre->id(), fre1->id());
    EXPECT_EQ(qre->lo_id(), fre1->lo_id());
}
*/

TEST_P(db_FileQuery_itest, test_simple) {
    query.filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(1)));
    r.reset(query.execute());

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_list_all_files) {
    r.reset(query.execute());

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_object) {
    query.filter(*xpr.attribute("what/object")->eq(*xpr.string("PVOL")));
    r.reset(query.execute());

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_xsize) {
    query.filter(*xpr.attribute("where/xsize")->eq(*xpr.int64_(2)));
    r.reset(query.execute());

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    query.filter(*xsize->eq(*xpr.int64_(1))->or_(*ysize->eq(*xpr.int64_(2))));
    r.reset(query.execute());

    ASSERT_EQ(r->size(), 3);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_combined_datetime) {
    DateTime min(2000, 1, 1, 12, 1);
    DateTime max(2001, 1, 1, 12, 0);
    expr::ExpressionPtr what_dt =
        xpr.add(
            *xpr.attribute("what/date"),
            *xpr.attribute("what/time")
        )->parentheses();

    query.filter(*what_dt->between(*xpr.datetime(min), *xpr.datetime(max)));
    r.reset(query.execute());

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("what/source:WMO");
    query.filter(*wmo_code->eq(*xpr.string("02666")));
    r.reset(query.execute());

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_node_or_node) {
    expr::AttributePtr node = xpr.attribute("what/source:_name");
    query.filter(*node->eq(*xpr.string("seang"))->or_(*node->eq(*xpr.string("sekkr"))));
    r.reset(query.execute());

    EXPECT_EQ(r->size(), 5);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_node_and_node) {
    expr::AttributePtr node = xpr.attribute("what/source:_name");
    query.filter(*node->eq(*xpr.string("seang"))->and_(*node->eq(*xpr.string("sekkr"))));
    r.reset(query.execute());

    EXPECT_EQ(0, r->size());
}

TEST_P(db_FileQuery_itest, test_filter_by_place) {
    query.filter(*xpr.attribute("what/source:PLC")->eq(*xpr.string("Ängelholm")));
    r.reset(query.execute());

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_is_stored) {
    bool result = false;
    ASSERT_NO_THROW(result = db->is_stored(td1));
    EXPECT_TRUE(result);
}

TEST_P(db_FileQuery_itest, test_has_nx_file) {
    bool result = false;
    oh5::hl::HlFile td("PVOL", Date(2000, 1, 10), Time(12, 0), src1);
    ASSERT_NO_THROW(result = db->is_stored(td));
    EXPECT_FALSE(result);
}

TEST_P(db_FileQuery_itest, test_query_like) {
    query.filter(*xpr.attribute("what/source:_name")->like("sea*"));
    r.reset(query.execute());

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_order_by) {
    query.order_by(*xpr.attribute("where/xsize"), FileQuery::DESC);
    r.reset(query.execute());

    EXPECT_EQ(5, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_limit) {
    query.limit(2);
    r.reset(query.execute());

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_query_missing) {

}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_FileQuery_itest_p,
                        db_FileQuery_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace db
} // namespace brfc
