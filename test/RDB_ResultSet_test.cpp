#include <gtest/gtest.h>

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

struct RDB_ResultSet_test : public testing::Test {
    RDB_ResultSet_test()
            : db(BRFC_TEST_DB_DSN) {
    }

    RelationalDatabase db;
};

} // namespace anonymous

TEST_F(RDB_ResultSet_test, size) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    EXPECT_EQ(r->size(), 1);
}

TEST_F(RDB_ResultSet_test, next) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    EXPECT_TRUE(r->next());
    EXPECT_TRUE(not r->next());
}

TEST_F(RDB_ResultSet_test, integer) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->integer(0), 1);
}

TEST_F(RDB_ResultSet_test, real) {
    shared_ptr<ResultSet> r = db.query("SELECT 1.1", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_NEAR(r->real(0), 1.1, 0.00001);
}

TEST_F(RDB_ResultSet_test, string) {
    shared_ptr<ResultSet> r = db.query("SELECT 'bla'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "bla");
}

TEST_F(RDB_ResultSet_test, boolean) {
    shared_ptr<ResultSet> r = db.query("SELECT true", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->boolean(0), true);
}

TEST_F(RDB_ResultSet_test, date) {
    shared_ptr<ResultSet> r = db.query("SELECT DATE '2001-01-02'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->date(0), QDate(2001, 1, 2));
}

TEST_F(RDB_ResultSet_test, time) {
    shared_ptr<ResultSet> r = db.query("SELECT TIME '12:00:05'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->time(0), QTime(12, 0, 5));
}

TEST_F(RDB_ResultSet_test, invalid_column) {
    shared_ptr<ResultSet> r = db.query("SELECT 1", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_THROW(r->integer(1), lookup_error);
}

#endif // BRFC_TEST_DB_DSN
