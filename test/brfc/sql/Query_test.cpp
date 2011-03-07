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
#include <gmock/gmock.h>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Query.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockDialect.hpp>

using ::testing::An;
using ::testing::Invoke;

namespace brfc {
namespace sql {

std::string
copystr(const std::string& str) {
    return str;
}

class sql_Query_test : public testing::Test {
  public:
    sql_Query_test()
        : query()
        , dialect() {
    }

    virtual void SetUp() {
        EXPECT_CALL(dialect, do_escape(An<const std::string&>()))
            .WillRepeatedly(Invoke(copystr));
    }

    Query query;
    BindMap binds;
    MockDialect dialect;
};

TEST_F(sql_Query_test, test_statement) {
    query.statement(":bind1, :bind2, :bind3");
    EXPECT_EQ(":bind1, :bind2, :bind3", query.statement());
}

TEST_F(sql_Query_test, test_bind) {
    binds.add(":bind", Variant());
    query.binds(binds);

    EXPECT_NO_THROW(query.bind(":bind", Variant(1)));
    ASSERT_TRUE(query.binds().has(":bind"));
    EXPECT_EQ(Variant(1), query.binds().get(":bind"));
}

TEST_F(sql_Query_test, test_bind_double) {
    binds.add(":bind", Variant());
    query.binds(binds);

    EXPECT_NO_THROW(query.bind(":bind", Variant(1.0f)));
    EXPECT_NO_THROW(query.bind(":bind", Variant(1)));
    ASSERT_TRUE(query.binds().has(":bind"));
    EXPECT_EQ(Variant(1), query.binds().get(":bind"));
}

TEST_F(sql_Query_test, test_bind_missing) {
    EXPECT_THROW(query.bind(":bind", Variant()), lookup_error);
}

TEST_F(sql_Query_test, test_replace_binds_missing_binds) {
    query.statement(":bind1 :bind2");
    binds.add(":bind1", Variant());
    query.binds(binds);

    EXPECT_THROW(query.replace_binds(dialect), lookup_error);
}

TEST_F(sql_Query_test, DISABLED_test_replace_binds_excessive_binds) {
    query.statement(":bind1");
    binds.add(":bind1", Variant());
    binds.add(":bind2", Variant());
    query.binds(binds);

    EXPECT_THROW(query.replace_binds(dialect), value_error);
}

TEST_F(sql_Query_test, test_replace_binds_find_simple_placeholders) {
    query.statement(":bind1 asd :bind2 qwe");
    binds.add(":bind1", Variant(1));
    binds.add(":bind2", Variant(2));
    query.binds(binds);

    std::string result;
    EXPECT_NO_THROW(result = query.replace_binds(dialect));
    EXPECT_EQ("1 asd 2 qwe", result);
}

TEST_F(sql_Query_test, test_replace_binds_find_complex_placeholders) {
    query.statement("(:bind1), :bind2, :bind_3+");
    binds.add(":bind1", Variant(1));
    binds.add(":bind2", Variant(2));
    binds.add(":bind_3", Variant(3));
    query.binds(binds);
    
    std::string result;
    EXPECT_NO_THROW(result = query.replace_binds(dialect));
    EXPECT_EQ("(1), 2, 3+", result);
}

TEST_F(sql_Query_test, test_replace_binds_large_replacement) {
    // replacement text is longer than placeholder
    query.statement(":bind1");
    binds.add(":bind1", Variant(1234567));
    query.binds(binds);
    
    std::string result;
    EXPECT_NO_THROW(result = query.replace_binds(dialect));
    EXPECT_EQ("1234567", result);
}

TEST_F(sql_Query_test, test_replace_binds_replacement_with_colon) {
    query.statement(":bind1 texttext :bind2 texttext :bind3");
    binds.add(":bind1", Variant(":a:b:c:d:e:"));
    binds.add(":bind2", Variant("a:b:c:d:e"));
    binds.add(":bind3", Variant(":a:b:c:d:e:"));
    query.binds(binds);

    std::string result;
    EXPECT_NO_THROW(result = query.replace_binds(dialect));
    EXPECT_EQ("':a:b:c:d:e:' texttext 'a:b:c:d:e' texttext ':a:b:c:d:e:'", result);
}

TEST_F(sql_Query_test, test_replace_binds_escaped_placeholder_marker) {
    query.statement("\\:notabind");

    std::string result;
    EXPECT_NO_THROW(result = query.replace_binds(dialect));
    EXPECT_EQ(result, "\\:notabind");
}

} // namespace sql
} // namespace brfc
