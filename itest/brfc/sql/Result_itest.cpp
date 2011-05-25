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

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/test/TestRDB.hpp>

#include <brfc/itest_config.hpp>
#include <brfc/ITestEnv.hpp>

#include <brfc/test_common.hpp>

using ::boost::shared_ptr;

namespace brfc {
namespace sql {

struct sql_Result_itest : public testing::TestWithParam<const char*> {
    sql_Result_itest()
            : db(ITestEnv::get_database(GetParam()))
            , conn(db->conn()) {
    }

    shared_ptr<Result> query(const std::string& stmt) {
        return shared_ptr<Result>(conn->execute(stmt));
    }
    
    test::TestRDB* db;
    shared_ptr<sql::Connection> conn;
};

TEST_P(sql_Result_itest, size) {
    shared_ptr<Result> r = query("SELECT 1");
    EXPECT_EQ(r->size(), 1);
}

TEST_P(sql_Result_itest, next) {
    shared_ptr<Result> r = query("SELECT 1");
    EXPECT_TRUE(r->next());
    EXPECT_TRUE(not r->next());
}

TEST_P(sql_Result_itest, int64) {
    shared_ptr<Result> r = query("SELECT 1");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->value_at(0).int64_(), 1);
}

TEST_P(sql_Result_itest, double_) {
    shared_ptr<Result> r = query("SELECT 1.1::real");
    ASSERT_TRUE(r->next());
    EXPECT_NEAR(r->value_at(0).double_(), 1.1, 0.00001);
}

TEST_P(sql_Result_itest, string) {
    shared_ptr<Result> r = query("SELECT 'bla'");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->value_at(0).string(), "bla");
}

TEST_P(sql_Result_itest, bool_) {
    shared_ptr<Result> r = query("SELECT true");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->value_at(0).bool_(), true);
}

TEST_P(sql_Result_itest, bool_false) {
    shared_ptr<Result> r = query("SELECT false");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->value_at(0).bool_(), false);
}

TEST_P(sql_Result_itest, date) {
    shared_ptr<Result> r = query("SELECT DATE '2001-01-02'");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->value_at(0).date(), Date(2001, 1, 2));
}

TEST_P(sql_Result_itest, time) {
    shared_ptr<Result> r = query("SELECT TIME '12:00:05'");
    ASSERT_TRUE(r->next());
    EXPECT_EQ(Time(12, 0, 5), r->value_at(0).time());
}

TEST_P(sql_Result_itest, invalid_column) {
    shared_ptr<Result> r = query("SELECT 1");
    ASSERT_TRUE(r->next());
    EXPECT_THROW(r->value_at(1), lookup_error);
}

TEST_P(sql_Result_itest, value_at_by_column) {
    shared_ptr<Result> r = query("SELECT 1 as i");
    ASSERT_TRUE(r->next());
    EXPECT_THROW(r->value_at("j"), lookup_error);
    EXPECT_NO_THROW(r->value_at("i"));
    EXPECT_EQ(1, r->value_at("i").int64_());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(sql_Result_itest_p,
                        sql_Result_itest,
                        ::testing::ValuesIn(TEST_DSNS));
#endif // BRFC_TEST_DSN_COUNT

} // namepsace sql
} // namespace brfc
