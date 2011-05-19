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

#include <brfc/db/FileEntry.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/FileResult.hpp>

#include <brfc/expr/ExpressionFactory.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test/TestRDB.hpp>
#include <brfc/test/TempH5File.hpp>

#include <brfc/test_common.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

namespace brfc {

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
            , query() {
    }

    void add_attribute(Oh5File& file,
                       std::string path,
                       const Oh5Scalar& value) {
        const std::string attr_name = path.substr(path.rfind('/') + 1);
        boost::erase_tail(path, attr_name.length() + 1);

        Oh5Node& g = static_cast<Oh5Group&>(file.root()).get_or_create_group(path);
        g.add(new Oh5Attribute(attr_name, value));
    }

    virtual void SetUp() {
        add_attribute(td1, "dataset1/where/xsize", Oh5Scalar(1));
        add_attribute(td1, "dataset1/where/ysize", Oh5Scalar(2));
        tf1.write(td1);
        td1.path(tf1.path());

        add_attribute(td2, "dataset1/where/xsize", Oh5Scalar(2));
        add_attribute(td2, "dataset1/where/ysize", Oh5Scalar(2));
        tf2.write(td2);
        td2.path(tf2.path());

        add_attribute(td3, "dataset1/where/xsize", Oh5Scalar(3));
        add_attribute(td3, "dataset2/where/xsize", Oh5Scalar(3));
        tf3.write(td3);
        td3.path(tf3.path());

        add_attribute(td4, "dataset1/where/xsize", Oh5Scalar(6));
        add_attribute(td4, "dataset1/where/ysize", Oh5Scalar(4));
        add_attribute(td4, "dataset2/where/ysize", Oh5Scalar(5));
        tf4.write(td4);
        td4.path(tf4.path());

        add_attribute(td5, "dataset1/where/xsize", Oh5Scalar(5));
        add_attribute(td5, "dataset1/where/ysize", Oh5Scalar(2));
        add_attribute(td5, "dataset2/where/xsize", Oh5Scalar(2));
        add_attribute(td5, "dataset2/where/ysize", Oh5Scalar(5));
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

    auto_ptr<FileEntry> entry(FileResult& r) {
        return auto_ptr<FileEntry>(r.entry());
    }
    
    ExpressionFactory xpr;
    std::string src1, src2;
    test::TestRDB* db;
    HlFile td1, td2, td3, td4, td5;
    test::TempH5File tf1, tf2, tf3, tf4, tf5;
    scoped_ptr<FileEntry> fe1, fe2, fe3, fe4, fe5;
    FileQuery query;
    scoped_ptr<FileResult> r;
};

TEST_P(db_FileQuery_itest, test_queried_entry) {
    query.filter(xpr.eq(xpr.attribute("file:uuid"), xpr.string(fe1->uuid())));
    r.reset(db->execute(query));

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    
    auto_ptr<FileEntry> e(r->entry());
    
    ASSERT_TRUE(e.get());
    
    EXPECT_EQ(td1.what_object(), e->what_object());
    EXPECT_EQ(td1.what_date(), e->what_date());
    EXPECT_EQ(td1.what_time(), e->what_time());
}

TEST_P(db_FileQuery_itest, test_simple) {
    query.filter(xpr.eq(xpr.attribute("where/xsize"), xpr.int64_(1)));
    r.reset(db->execute(query));

    EXPECT_EQ(1, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_list_all_files) {
    r.reset(db->execute(query));

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
    query.filter(xpr.eq(xpr.attribute("what/object"), xpr.string("PVOL")));
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_xsize) {
    query.filter(xpr.eq(xpr.attribute("where/xsize"), xpr.int64_(2)));
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_xsize_or_ysize) {
    Expression xsize = xpr.attribute("where/xsize");
    Expression ysize = xpr.attribute("where/ysize");
    query.filter(
        xpr.or_(
            xpr.eq(xpr.attribute("where/xsize"), xpr.int64_(1)),
            xpr.eq(xpr.attribute("where/ysize"), xpr.int64_(2))
        )
    );
    r.reset(db->execute(query));

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
    Expression what_dt =
        xpr.add(xpr.attribute("what/date"), xpr.attribute("what/time"));

    query.filter(xpr.between(what_dt, xpr.datetime(min), xpr.datetime(max)));
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_wmo_code) {
    Expression wmo_code = xpr.attribute("what/source:WMO");
    query.filter(xpr.eq(wmo_code, xpr.string("02666")));
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_filter_by_node_or_node) {
    Expression node = xpr.attribute("what/source:_name");
    query.filter(
        xpr.or_(
            xpr.eq(node, xpr.string("seang")),
            xpr.eq(node, xpr.string("sekkr"))
        )
    );
    r.reset(db->execute(query));

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
    Expression node = xpr.attribute("what/source:_name");
    query.filter(
        xpr.and_(
            xpr.eq(node, xpr.string("seang")),
            xpr.eq(node, xpr.string("sekkr"))
        )
    );
    r.reset(db->execute(query));

    EXPECT_EQ(0, r->size());
}

TEST_P(db_FileQuery_itest, test_filter_by_place) {
    query.filter(xpr.eq(xpr.attribute("what/source:PLC"), xpr.string("Ängelholm")));
    r.reset(db->execute(query));

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
    HlFile td("PVOL", Date(2000, 1, 10), Time(12, 0), src1);
    ASSERT_NO_THROW(result = db->is_stored(td));
    EXPECT_FALSE(result);
}

TEST_P(db_FileQuery_itest, test_query_like) {
    query.filter(xpr.like(xpr.attribute("what/source:_name"), "sea*"));
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_order_by) {
    query.order_by(xpr.attribute("where/xsize"), FileQuery::DESC);
    r.reset(db->execute(query));

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
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe1->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_skip) {
    query.skip(3);
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_skip_with_limit) {
    query.skip(1);
    query.limit(3);
    r.reset(db->execute(query));

    EXPECT_EQ(3, r->size());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe2->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe3->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(fe4->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_in) {
    Expression l;
    l.push_back(xpr.string("CVOL"));
    l.push_back(xpr.string("SCAN"));

    query.filter(xpr.in(xpr.attribute("what/object"), xpr.list(l)));
    r.reset(db->execute(query));

    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    ASSERT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    ASSERT_EQ(fe5->uuid(), entry(*r)->uuid());
}

TEST_P(db_FileQuery_itest, test_not_in) {
    Expression l;
    l.push_back(xpr.int64_(2));
    l.push_back(xpr.int64_(4));

    query.filter(xpr.not_(xpr.in(xpr.attribute("where/ysize"), xpr.list(l))));
    r.reset(db->execute(query));
    
    EXPECT_EQ(2, r->size());
    ASSERT_TRUE(r->next());
    ASSERT_EQ(fe4->uuid(), entry(*r)->uuid());
    ASSERT_TRUE(r->next());
    ASSERT_EQ(fe5->uuid(), entry(*r)->uuid());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(db_FileQuery_itest_p,
                        db_FileQuery_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc

