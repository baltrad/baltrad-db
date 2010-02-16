#include <gtest/gtest.h>

#include <brfc/TestRDB.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/File.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/BinaryOperator.hpp>

#include <QtCore/QVariant>
#include <QtCore/QDate>
#include <QtCore/QTime>

#include <algorithm>
#include <vector>

#include "config.hpp"
#include "common.hpp"

using namespace brfc;

namespace {

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
struct RDB_Query_test : public testing::TestWithParam<const char*> {
    RDB_Query_test()
            : xpr()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(TestRDBEnv::get_database(GetParam()))
            , td1("PVOL", QDate(2000, 1, 1), QTime(12, 0), src1)
            , td2("PVOL", QDate(2000, 1, 1), QTime(12, 1), src2)
            , td3("PVOL", QDate(2000, 1, 1), QTime(12, 2), src1)
            , td4("CVOL", QDate(2001, 1, 1), QTime(12, 0), src2)
            , td5("SCAN", QDate(2002, 2, 1), QTime(12, 0), src1)
            , query(db) {
    }

    virtual void SetUp() {
        td1.data_object("/do1", true).add_attribute("where/xsize", Variant(1));
        td1.data_object("/do1", true).add_attribute("where/ysize", Variant(2));

        td2.data_object("/do1", true).add_attribute("where/xsize", Variant(2));
        td2.data_object("/do1", true).add_attribute("where/ysize", Variant(2));

        td3.data_object("/do1", true).add_attribute("where/xsize", Variant(3));
        td3.data_object("/do2", true).add_attribute("where/xsize", Variant(3));

        td4.data_object("/do1", true).add_attribute("where/xsize", Variant(6));
        td4.data_object("/do1", true).add_attribute("where/ysize", Variant(4));
        td4.data_object("/do2", true).add_attribute("where/ysize", Variant(5));

        td5.data_object("/do1", true).add_attribute("where/xsize", Variant(5));
        td5.data_object("/do1", true).add_attribute("where/ysize", Variant(2));
        td5.data_object("/do2", true).add_attribute("where/xsize", Variant(2));
        td5.data_object("/do2", true).add_attribute("where/ysize", Variant(5));

        db->save_file("td1", td1);
        db->save_file("td2", td2);
        db->save_file("td3", td3);
        db->save_file("td4", td4);
        db->save_file("td5", td5);
    }

    virtual void TearDown() {
        db->clean();
    }
    
    expr::Factory xpr;
    std::string src1, src2;
    TestRDB* db;
    File td1, td2, td3, td4, td5;
    Query query;
};

}

TEST_P(RDB_Query_test, test_simple) {
    shared_ptr<ResultSet> r = 
            query.fetch(xpr.attribute("path"))
                 .filter(xpr.attribute("where/xsize")->eq(xpr.integer(1)))
                 .execute();
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "td1");
    ASSERT_TRUE(not r->next());
}

typedef std::vector<std::string> StringVec;

StringVec
extract_strings_at(ResultSet& r, int pos) {
    std::vector<std::string> vec;
    while (r.next()) {
        vec.push_back(r.string(pos));
    }
    return vec;
}

template<typename C, typename V>
bool contains(const C& container, const V& val) {
    return std::find(container.begin(), container.end(), val) != container.end();
}

TEST_P(RDB_Query_test, test_list_all_files) {
    shared_ptr<ResultSet> r = query.fetch(xpr.attribute("path")).execute();

    EXPECT_EQ(r->size(), 5);
    StringVec v = extract_strings_at(*r, 0);

    EXPECT_TRUE(contains(v, "td1"));
    EXPECT_TRUE(contains(v, "td2"));
    EXPECT_TRUE(contains(v, "td3"));
    EXPECT_TRUE(contains(v, "td4"));
    EXPECT_TRUE(contains(v, "td5"));
}

TEST_P(RDB_Query_test, test_filter_by_object) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xpr.attribute("what/object")->eq(xpr.string("PVOL")))
             .execute();

    EXPECT_EQ(r->size(), 3);
    StringVec v = extract_strings_at(*r, 0);

    EXPECT_TRUE(contains(v, "td1"));
    EXPECT_TRUE(contains(v, "td2"));
    EXPECT_TRUE(contains(v, "td3"));
}

TEST_P(RDB_Query_test, test_fetch_xsize_filtering_by_xsize) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("where/xsize"))
             .filter(xpr.attribute("where/xsize")->eq(xpr.integer(2)))
             .execute();
    EXPECT_EQ(r->size(), 2);
}

TEST_P(RDB_Query_test, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xsize->eq(xpr.integer(1))->or_(ysize->eq(xpr.integer(2))))
             .execute();

    EXPECT_EQ(r->size(), 3);
    StringVec v = extract_strings_at(*r, 0);

    EXPECT_TRUE(contains(v, "td1"));
    EXPECT_TRUE(contains(v, "td2"));
    EXPECT_TRUE(contains(v, "td5"));
}

TEST_P(RDB_Query_test, test_filter_by_xsize_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<ResultSet> r = 
        query.fetch(xpr.attribute("path"))
             .distinct(true)
             .filter(xsize->eq(xpr.integer(3)))
             .execute();
    EXPECT_EQ(r->size(), 1);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "td3");
}

TEST_P(RDB_Query_test, test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("src_WMO");
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(wmo_code->eq(xpr.integer(2666)))
             .execute();

    EXPECT_EQ(r->size(), 2);
    StringVec v = extract_strings_at(*r, 0);

    EXPECT_TRUE(contains(v, "td2"));
    EXPECT_TRUE(contains(v, "td4"));
}

TEST_P(RDB_Query_test, test_select_by_place) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xpr.attribute("src_PLC")->eq(xpr.string("Ängelholm")))
             .execute();

    EXPECT_EQ(r->size(), 3);
    StringVec v = extract_strings_at(*r, 0);

    EXPECT_TRUE(contains(v, "td1"));
    EXPECT_TRUE(contains(v, "td3"));
    EXPECT_TRUE(contains(v, "td5"));
}

TEST_P(RDB_Query_test, test_has_file) {
    bool result = false;
    ASSERT_NO_THROW(result = db->has_file(td1));
    EXPECT_TRUE(result);
}

TEST_P(RDB_Query_test, test_has_nx_file) {
    bool result = false;
    File td("PVOL", QDate(2000, 1, 10), QTime(12, 0), src1);
    ASSERT_NO_THROW(result = db->has_file(td));
    EXPECT_FALSE(result);
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(RDB_Query_test_p,
                        RDB_Query_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT
