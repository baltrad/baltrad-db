#include <gtest/gtest.h>

#include <brfc/AttributeMapper.hpp>
#include <brfc/RelationalDatabase.hpp>
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

using namespace brfc;

#ifdef BRFC_TEST_DB_DSN
namespace {

struct Query_test : public testing::Test {
    Query_test()
            : xpr()
            , mapper()
            , src("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , db(BRFC_TEST_DB_DSN)
            , td1("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , td2("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , td3("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , query(&db, &mapper) {
        db.populate_attribute_mapper(mapper);
        td1.data_object("/bla", true).add_attribute("where/xsize", QVariant(static_cast<int64_t>(1)));
        td1.data_object("/bla", true).add_attribute("where/ysize", QVariant(static_cast<int64_t>(2)));

        td2.data_object("/bla", true).add_attribute("where/xsize", QVariant(static_cast<int64_t>(2)));
        td2.data_object("/bla", true).add_attribute("where/ysize", QVariant(static_cast<int64_t>(2)));

        td3.data_object("/bla", true).add_attribute("where/xsize", QVariant(static_cast<int64_t>(3)));
        td3.data_object("/bla2", true).add_attribute("where/xsize", QVariant(static_cast<int64_t>(3)));

        td1.root().add_attribute("path", "test_data_1");
        td2.root().add_attribute("path", "test_data_2");
        td3.root().add_attribute("path", "test_data_3");

        db.save_file(td1, mapper);
        db.save_file(td2, mapper);
        db.save_file(td3, mapper);
    }

    ~Query_test() {
        db.clean();
    }

    expr::Factory xpr;
    AttributeMapper mapper;
    std::string src;
    RelationalDatabase db;
    File td1, td2, td3;
    Query query;
};

}

TEST_F(Query_test, test_simple) {
    shared_ptr<ResultSet> r = 
            query.fetch(xpr.attribute("path"))
                 .filter(xpr.attribute("where/xsize")->eq(xpr.integer(1)))
                 .execute();
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "test_data_1");
    ASSERT_TRUE(not r->next());
}

TEST_F(Query_test, test_list_all_files) {
    shared_ptr<ResultSet> r = query.fetch(xpr.attribute("path")).execute();
    ASSERT_EQ(r->size(), 3);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "test_data_1");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "test_data_2");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "test_data_3");
    ASSERT_TRUE(not r->next());
}

TEST_F(Query_test, test_select_xsize) {
    shared_ptr<ResultSet> r = query.fetch(xpr.attribute("where/xsize"))
                       .filter(xpr.attribute("where/xsize")->eq(xpr.integer(1)))
                       .execute();
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->integer(0), 1);
    ASSERT_TRUE(not r->next());
}

TEST_F(Query_test, test_select_on_double_attr) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    shared_ptr<ResultSet> r = query.fetch(xpr.attribute("path"))
                       .filter(xsize->eq(xpr.integer(1))->or_(ysize->eq(xpr.integer(2))))
                       .execute();
    ASSERT_EQ(r->size(), 2);
    r->next();
    std::string first = r->string(0);
    r->next();
    std::string second = r->string(0);
    EXPECT_NE(first, second);
}

TEST_F(Query_test, test_select_not_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<ResultSet> r =
            query.fetch(xpr.attribute("path"))
                 .filter(xsize->eq(xpr.integer(3)))
                 .execute();
    ASSERT_EQ(r->size(), 2);
    r->next();
    std::string first = r->string(0);
    r->next();
    std::string second = r->string(0);
    EXPECT_EQ(first, second);
}

TEST_F(Query_test, test_select_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<ResultSet> r = 
            query.fetch(xpr.attribute("path"))
                 .distinct(true)
                 .filter(xsize->eq(xpr.integer(3)))
                 .execute();
    ASSERT_EQ(r->size(), 1);
    r->next();
    EXPECT_EQ("test_data_3", r->string(0));
}


TEST_F(Query_test, test_select_xsize2) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<ResultSet> r =
            query.fetch(xsize)
                 .filter(xsize->eq(xpr.integer(1))->or_(xsize->eq(xpr.integer(2))))
                 .execute();
    ASSERT_EQ(r->size(), 2);

}

TEST_F(Query_test, test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("src_WMO");
    shared_ptr<ResultSet> r =
            query.fetch(xpr.attribute("path"))
                 .filter(wmo_code->eq(xpr.integer(2606)))
                 .execute();
    ASSERT_EQ(r->size(), 3);

}

#endif // BRFC_TEST_DB_DSN
