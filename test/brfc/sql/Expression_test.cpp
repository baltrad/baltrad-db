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

#include <string>
#include <brfc/Variant.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

#include <brfc/test_common.hpp>

namespace brfc {
namespace sql {

class sql_Expression_test: public testing::Test {
  public:
    Factory xpr;
};


TEST_F(sql_Expression_test, test_utf_string_literal) {
    LiteralPtr lit = xpr.string("öäü");
    std::string s = lit->value().string();
    EXPECT_EQ("öäü", s);
}

TEST_F(sql_Expression_test, test_join_contains) {

    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    AliasPtr t2a = t2->alias("t2a");
    TablePtr t3 = Table::create("t3");
    JoinPtr j = t1->join(t2a, xpr.int64_(1))->join(t3, xpr.int64_(1));
    EXPECT_TRUE(j->contains(t1));
    EXPECT_TRUE(not j->contains(t2));
    EXPECT_TRUE(j->contains(t2a));
    EXPECT_TRUE(j->contains(t3));
}

} // namespace sql
} // namespace brfc
