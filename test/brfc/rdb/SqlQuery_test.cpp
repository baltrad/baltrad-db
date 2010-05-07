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

#include <brfc/rdb/SqlQuery.hpp>

#include "../common.hpp"
#include "MockConnection.hpp"

namespace brfc {
namespace rdb {

class rdb_SqlQuery_test : public testing::Test {
  public:
    rdb_SqlQuery_test()
        : query()
        , conn() {
    }

    SqlQuery query;
    MockConnection conn;
};

TEST_F(rdb_SqlQuery_test, test_replace_binds_missing_binds) {
    query.statement(":bind1 :bind2");
    query.binds().add(":bind1", Variant());

    EXPECT_THROW(query.replace_binds(conn), value_error);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_excessive_binds) {
    query.statement(":bind1");
    query.binds().add(":bind1", Variant());
    query.binds().add(":bind2", Variant());

    EXPECT_THROW(query.replace_binds(conn), value_error);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_find_simple_placeholders) {
    query.statement(":bind1 asd :bind2 qwe");
    query.binds().add(":bind1", Variant(1));
    query.binds().add(":bind2", Variant(2));

    String result;
    EXPECT_NO_THROW(result = query.replace_binds(conn));
    EXPECT_EQ("1 asd 2 qwe", result);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_find_complex_placeholders) {
    query.statement("(:bind1), :bind2, :bind_3+");
    query.binds().add(":bind1", Variant(1));
    query.binds().add(":bind2", Variant(2));
    query.binds().add(":bind_3", Variant(3));
    
    String result;
    EXPECT_NO_THROW(result = query.replace_binds(conn));
    EXPECT_EQ("(1), 2, 3+", result);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_large_replacement) {
    // replacement text is longer than placeholder
    query.statement(":bind1");
    query.binds().add(":bind1", Variant(1234567));
    
    String result;
    EXPECT_NO_THROW(result = query.replace_binds(conn));
    EXPECT_EQ("1234567", result);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_replacement_with_colon) {
    query.statement(":bind1 texttext :bind2 texttext :bind3");
    query.binds().add(":bind1", Variant(":a:b:c:d:e:"));
    query.binds().add(":bind2", Variant("a:b:c:d:e"));
    query.binds().add(":bind3", Variant(":a:b:c:d:e:"));

    String result;
    EXPECT_NO_THROW(result = query.replace_binds(conn));
    EXPECT_EQ("':a:b:c:d:e:' texttext 'a:b:c:d:e' texttext ':a:b:c:d:e:'", result);
}

TEST_F(rdb_SqlQuery_test, test_replace_binds_escaped_placeholder_marker) {
    query.statement("\\:notabind");

    String result;
    EXPECT_NO_THROW(result = query.replace_binds(conn));
    EXPECT_EQ(result, "\\:notabind");
}

} // namespace rdb
} // namespace brfc
