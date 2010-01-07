#include <boost/test/unit_test.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/RelationalDatabase.hpp>
#include <brfc/ResultSet.hpp>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "common.hpp"

using namespace brfc;

#ifdef BRFC_TEST_DB_DSN
namespace {

struct Fixture {
    Fixture()
            : db(BRFC_TEST_DB_DSN) {
    }

    RelationalDatabase db;
};

} // namespace anonymous

BOOST_FIXTURE_TEST_SUITE(ResultSet_test, Fixture)

BOOST_AUTO_TEST_CASE(size) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    BOOST_CHECK_EQUAL(r->size(), 1);
}

BOOST_AUTO_TEST_CASE(next) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    BOOST_CHECK(r->next());
    BOOST_CHECK(not r->next());
}

BOOST_AUTO_TEST_CASE(integer) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_EQUAL(r->integer(0), 1);
}

BOOST_AUTO_TEST_CASE(real) {
    shared_ptr<ResultSet> r = db.query("SELECT 1.1", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_CLOSE(r->real(0), 1.1, 0.00001);
}

BOOST_AUTO_TEST_CASE(string) {
    shared_ptr<ResultSet> r = db.query("SELECT 'bla'", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_EQUAL(r->string(0), "bla");
}

BOOST_AUTO_TEST_CASE(boolean) {
    shared_ptr<ResultSet> r = db.query("SELECT true", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_EQUAL(r->boolean(0), true);
}

BOOST_AUTO_TEST_CASE(date) {
    shared_ptr<ResultSet> r = db.query("SELECT DATE '2001-01-02'", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_EQUAL(r->date(0), QDate(2001, 1, 2));
}

BOOST_AUTO_TEST_CASE(time) {
    shared_ptr<ResultSet> r = db.query("SELECT TIME '12:00:05'", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_EQUAL(r->time(0), QTime(12, 0, 5));
}

BOOST_AUTO_TEST_CASE(invalid_column) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    BOOST_REQUIRE(r->next());
    BOOST_CHECK_THROW(r->integer(1), lookup_error);
}

BOOST_AUTO_TEST_SUITE_END()

#endif // BRFC_TEST_DB_DSN
