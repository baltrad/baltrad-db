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
#include <gmock/gmock.h>

#include <QtCore/QString>

#include <brfc/ResultSet.hpp>
#include <brfc/Variant.hpp>
#include <brfc/rdb/Connection.hpp>
#include <brfc/rdb/SqlQuery.hpp>

#include "../common.hpp"
#include "../MockResultSet.hpp"

using testing::Return;
using testing::Throw;
using testing::_;

namespace brfc {
namespace rdb {

class MockConnection : public Connection {
  public:
    MOCK_METHOD0(do_begin, void());
    MOCK_METHOD0(do_commit, void());
    MOCK_METHOD0(do_rollback, void());
    MOCK_METHOD1(do_execute, shared_ptr<ResultSet>(const QString&));
    MOCK_METHOD0(do_in_transaction, bool());
};

class rdb_Connection_test : public testing::Test {
  public:
    rdb_Connection_test()
        : conn() {
    }

    MockConnection conn;
};

TEST_F(rdb_Connection_test, test_no_transaction_execute) {
    QString stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false))  // execute()
        .WillOnce(Return(false))  // begin()
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_begin());
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResultSet>()));
    EXPECT_CALL(conn, do_commit());

    conn.execute(stmt);
}

TEST_F(rdb_Connection_test, test_no_transaction_excute_throws) {
    QString stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false)) // execute()
        .WillOnce(Return(false)) // being()
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_begin());
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(conn, do_rollback());

    EXPECT_THROW(conn.execute(stmt), db_error);
}

TEST_F(rdb_Connection_test, test_in_transaction_execute) {
    QString stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResultSet>()));
    
    conn.execute(stmt);
}

TEST_F(rdb_Connection_test, test_in_transaction_execute_throws) {
    QString stmt("query");
    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Throw(std::runtime_error("")));
    
    EXPECT_THROW(conn.execute(stmt), db_error);
}

TEST_F(rdb_Connection_test, test_replace_binds_missing_binds) {
    QString stmt(":bind1 :bind2");
    BindMap binds;
    binds.add(":bind1", Variant());

    EXPECT_THROW(conn.replace_binds(stmt, binds), value_error);
}

TEST_F(rdb_Connection_test, test_replace_binds_excessive_binds) {
    QString stmt(":bind1");
    BindMap binds;
    binds.add(":bind1", Variant());
    binds.add(":bind2", Variant());

    EXPECT_THROW(conn.replace_binds(stmt, binds), value_error);
}

TEST_F(rdb_Connection_test, test_replace_binds_find_simple_placeholders) {
    QString stmt(":bind1 asd :bind2 qwe");
    BindMap binds;
    binds.add(":bind1", Variant(1));
    binds.add(":bind2", Variant(2));

    QString result;
    EXPECT_NO_THROW(result = conn.replace_binds(stmt, binds));
    EXPECT_EQ("1 asd 2 qwe", result);
}

TEST_F(rdb_Connection_test, test_replace_binds_find_complex_placeholders) {
    QString stmt("(:bind1), :bind2, :bind_3+");
    BindMap binds;
    binds.add(":bind1", Variant(1));
    binds.add(":bind2", Variant(2));
    binds.add(":bind_3", Variant(3));
    
    QString result;
    EXPECT_NO_THROW(result = conn.replace_binds(stmt, binds));
    EXPECT_EQ("(1), 2, 3+", result);
}

TEST_F(rdb_Connection_test, test_replace_binds_large_replacement) {
    // replacement text is longer than placeholder
    QString stmt(":bind1");
    BindMap binds;
    binds.add(":bind1", Variant(1234567));
    
    QString result;
    EXPECT_NO_THROW(result = conn.replace_binds(stmt, binds));
    EXPECT_EQ("1234567", result);
}

TEST_F(rdb_Connection_test, test_replace_binds_replacement_with_colon) {
    QString stmt(":bind1 texttext :bind2 texttext :bind3");
    BindMap binds;
    binds.add(":bind1", Variant(":a:b:c:d:e:"));
    binds.add(":bind2", Variant("a:b:c:d:e"));
    binds.add(":bind3", Variant(":a:b:c:d:e:"));

    QString result;
    EXPECT_NO_THROW(result = conn.replace_binds(stmt, binds));
    EXPECT_EQ("':a:b:c:d:e:' texttext 'a:b:c:d:e' texttext ':a:b:c:d:e:'", result);
}

TEST_F(rdb_Connection_test, test_replace_binds_escaped_placeholder_marker) {
    QString stmt("\\:notabind");
    BindMap binds;

    QString result;
    EXPECT_NO_THROW(result = conn.replace_binds(stmt, binds));
    EXPECT_EQ(result, "\\:notabind");
}

TEST_F(rdb_Connection_test, test_variant_to_string_string) {
    EXPECT_EQ("'qweqwe'", conn.variant_to_string(Variant("qweqwe")));
}

TEST_F(rdb_Connection_test, test_variant_to_string_integer) {
    EXPECT_EQ("1", conn.variant_to_string(Variant(1)));
}

TEST_F(rdb_Connection_test, test_variant_to_string_float) {
    EXPECT_EQ("0.5", conn.variant_to_string(Variant(0.5)));
}

TEST_F(rdb_Connection_test, test_variant_to_string_date) {
    EXPECT_EQ("'2001-05-01'", conn.variant_to_string(Variant(QDate(2001, 5, 1))));
}

TEST_F(rdb_Connection_test, test_variant_to_string_time) {
    EXPECT_EQ("'13:05:59.001'", conn.variant_to_string(Variant(QTime(13, 5, 59, 1))));
}

TEST_F(rdb_Connection_test, test_variant_to_string_null) {
    EXPECT_EQ("NULL", conn.variant_to_string(Variant()));
}

TEST_F(rdb_Connection_test, test_begin) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));
    EXPECT_CALL(conn, do_begin());

    EXPECT_NO_THROW(conn.begin());
}

TEST_F(rdb_Connection_test, test_begin_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));

    EXPECT_THROW(conn.begin(), db_error);
}

TEST_F(rdb_Connection_test, test_rollback_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_rollback());

    EXPECT_NO_THROW(conn.rollback());
}

TEST_F(rdb_Connection_test, test_rollback_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.rollback(), db_error);
}

TEST_F(rdb_Connection_test, test_commit_in_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(true));
    EXPECT_CALL(conn, do_commit());

    EXPECT_NO_THROW(conn.commit());
}

TEST_F(rdb_Connection_test, test_commit_no_transaction) {
    EXPECT_CALL(conn, do_in_transaction())
        .WillOnce(Return(false));

    EXPECT_THROW(conn.commit(), db_error);
}

TEST_F(rdb_Connection_test, test_execute_sqlquery) {
    QString stmt("query");
    BindMap binds;
    SqlQuery query(stmt, binds);

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(stmt))
        .WillOnce(Return(make_shared<MockResultSet>()));
    
    conn.execute(query);
}

TEST_F(rdb_Connection_test, test_execute_replaces_binds) {
    QString stmt(":bind");
    BindMap binds;
    binds.add(":bind", Variant(1));

    ON_CALL(conn, do_execute(_))
        .WillByDefault(Return(shared_ptr<ResultSet>()));

    EXPECT_CALL(conn, do_in_transaction())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(conn, do_execute(QString("1")))
        .WillOnce(Return(shared_ptr<ResultSet>()));
    
    conn.execute(stmt, binds);
}

} // namespace rdb
} // namespace brfc
