#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/Variant.hpp>

#include <brfc/test/TestRDB.hpp>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QTime>

#include "config.hpp"
#include "../common.hpp"

namespace brfc {
namespace rdb {

struct rdb_ResultSet_test : public testing::TestWithParam<const char*> {
    rdb_ResultSet_test()
            : db(TestRDBEnv::get_database(GetParam())) {
    }

    test::TestRDB* db;
};

TEST_P(rdb_ResultSet_test, size) {
    shared_ptr<ResultSet> r = db->query("SELECT 1", BindMap());
    EXPECT_EQ(r->size(), 1);
}

TEST_P(rdb_ResultSet_test, next) {
    shared_ptr<ResultSet> r = db->query("SELECT 1", BindMap());
    EXPECT_TRUE(r->next());
    EXPECT_TRUE(not r->next());
}

TEST_P(rdb_ResultSet_test, integer) {
    shared_ptr<ResultSet> r = db->query("SELECT 1", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->integer(0), 1);
}

TEST_P(rdb_ResultSet_test, real) {
    shared_ptr<ResultSet> r = db->query("SELECT 1.1::real", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_NEAR(r->real(0), 1.1, 0.00001);
}

TEST_P(rdb_ResultSet_test, string) {
    shared_ptr<ResultSet> r = db->query("SELECT 'bla'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "bla");
}

TEST_P(rdb_ResultSet_test, boolean) {
    shared_ptr<ResultSet> r = db->query("SELECT true", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->boolean(0), true);
}

TEST_P(rdb_ResultSet_test, date) {
    shared_ptr<ResultSet> r = db->query("SELECT DATE '2001-01-02'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->date(0), QDate(2001, 1, 2));
}

TEST_P(rdb_ResultSet_test, time) {
    shared_ptr<ResultSet> r = db->query("SELECT TIME '12:00:05'", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->time(0), QTime(12, 0, 5));
}

TEST_P(rdb_ResultSet_test, invalid_column) {
    shared_ptr<ResultSet> r = db->query("SELECT 1", BindMap());
    ASSERT_TRUE(r->next());
    EXPECT_THROW(r->integer(1), lookup_error);
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(rdb_ResultSet_test_p,
                        rdb_ResultSet_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namepsace rdb
} // namespace brfc
