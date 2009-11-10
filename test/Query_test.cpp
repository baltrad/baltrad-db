#include <boost/test/unit_test.hpp>

#include <brfc/AttributeMapper.hpp>
#include <brfc/Database.hpp>
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

struct Fixture {
    Fixture()
            : xpr()
            , mapper()
            , src("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , db(BRFC_TEST_DB_DSN)
            , td1("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , td2("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , td3("PVOL", "H5rad 2.0", QDate(2000, 1, 1), QTime(12, 0), src)
            , query(&db, &mapper) {
        db.populate_attribute_mapper(mapper);
        td1.data_object("/bla", true).add_attribute("xsize", QVariant(static_cast<int64_t>(1)));
        td1.data_object("/bla", true).add_attribute("ysize", QVariant(static_cast<int64_t>(2)));

        td2.data_object("/bla", true).add_attribute("xsize", QVariant(static_cast<int64_t>(2)));
        td2.data_object("/bla", true).add_attribute("ysize", QVariant(static_cast<int64_t>(2)));

        td3.data_object("/bla", true).add_attribute("xsize", QVariant(static_cast<int64_t>(3)));
        td3.data_object("/bla2", true).add_attribute("xsize", QVariant(static_cast<int64_t>(3)));

        td1.root().add_attribute("path", "test_data_1");
        td2.root().add_attribute("path", "test_data_2");
        td3.root().add_attribute("path", "test_data_3");

        db.save_recurse(td1, mapper);
        db.save_recurse(td2, mapper);
        db.save_recurse(td3, mapper);
    }

    ~Fixture() {
        db.clean();
    }

    expr::Factory xpr;
    AttributeMapper mapper;
    std::string src;
    Database db;
    File td1, td2, td3;
    Query query;
};

}

BOOST_FIXTURE_TEST_SUITE(Query_test, Fixture)

BOOST_AUTO_TEST_CASE(test_simple) {
    ResultSet r = query.fetch(xpr.attribute("path"))
                       .filter(xpr.attribute("xsize")->eq(xpr.integer(1)))
                       .execute();
    BOOST_REQUIRE(r.next());
    BOOST_CHECK_EQUAL(r.string(0), "test_data_1");
    BOOST_REQUIRE(not r.next());
}

BOOST_AUTO_TEST_CASE(test_list_all_files) {
    ResultSet r = query.fetch(xpr.attribute("path")).execute();
    BOOST_REQUIRE(r.next());
    BOOST_CHECK_EQUAL(r.string(0), "test_data_1");
    BOOST_REQUIRE(r.next());
    BOOST_CHECK_EQUAL(r.string(0), "test_data_2");
    BOOST_REQUIRE(r.next());
    BOOST_CHECK_EQUAL(r.string(0), "test_data_3");
    BOOST_REQUIRE(not r.next());
}

BOOST_AUTO_TEST_CASE(test_select_xsize) {
    ResultSet r = query.fetch(xpr.attribute("xsize"))
                       .filter(xpr.attribute("xsize")->eq(xpr.integer(1)))
                       .execute();
    BOOST_REQUIRE(r.next());
    BOOST_CHECK_EQUAL(r.integer(0), 1);
    BOOST_REQUIRE(not r.next());
}

BOOST_AUTO_TEST_CASE(test_select_on_double_attr) {
    expr::AttributePtr xsize = xpr.attribute("xsize");
    expr::AttributePtr ysize = xpr.attribute("ysize");
    ResultSet r = query.fetch(xpr.attribute("path"))
                       .filter(xsize->eq(xpr.integer(1))->or_(ysize->eq(xpr.integer(2))))
                       .execute();
    BOOST_REQUIRE_EQUAL(r.size(), 2);
    r.next();
    std::string first = r.string(0);
    r.next();
    std::string second = r.string(0);
    BOOST_CHECK_NE(first, second);
}

BOOST_AUTO_TEST_CASE(test_select_not_distinct) {
    expr::AttributePtr xsize = xpr.attribute("xsize");
    ResultSet r = query.fetch(xpr.attribute("path"))
                       .filter(xsize->eq(xpr.integer(3)))
                       .execute();
    BOOST_REQUIRE_EQUAL(r.size(), 2);
    r.next();
    std::string first = r.string(0);
    r.next();
    std::string second = r.string(0);
    BOOST_CHECK_EQUAL(first, second);
}

BOOST_AUTO_TEST_CASE(test_select_distinct) {
    expr::AttributePtr xsize = xpr.attribute("xsize");
    ResultSet r = query.fetch(xpr.attribute("path"))
                       .distinct()
                       .filter(xsize->eq(xpr.integer(3)))
                       .execute();
    BOOST_REQUIRE_EQUAL(r.size(), 1);
    r.next();
    BOOST_CHECK_EQUAL("test_data_3", r.string(0));
}


BOOST_AUTO_TEST_CASE(test_select_xsize2) {
    expr::AttributePtr xsize = xpr.attribute("xsize");
    ResultSet r = query.fetch(xsize)
                       .filter(xsize->eq(xpr.integer(1))->or_(xsize->eq(xpr.integer(2))))
                       .execute();
    BOOST_REQUIRE_EQUAL(r.size(), 2);

}

BOOST_AUTO_TEST_CASE(test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("src_WMO");
    ResultSet r = query.fetch(xpr.attribute("path"))
                       .filter(wmo_code->eq(xpr.integer(2606)))
                       .execute();
    BOOST_REQUIRE_EQUAL(r.size(), 3);

}

BOOST_AUTO_TEST_SUITE_END()

#endif // BRFC_TEST_DB_DSN
