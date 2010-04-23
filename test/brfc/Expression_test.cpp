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

#include <brfc/Variant.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/rdb/Alias.hpp>
#include <brfc/rdb/Column.hpp>
#include <brfc/rdb/FromClause.hpp>
#include <brfc/rdb/Join.hpp>
#include <brfc/rdb/Select.hpp>
#include <brfc/rdb/Table.hpp>

#include "common.hpp"

#include <QtCore/QString>

namespace brfc {
namespace expr {

class Expression_test: public testing::Test {
  public:
    Factory xpr;
};


TEST_F(Expression_test, test_utf_string_literal) {
    LiteralPtr lit = xpr.string("öäü");
    QString s = lit->value().string();
    EXPECT_EQ(s.length(), 3);
}

TEST_F(Expression_test, test_join_contains) {
    using namespace ::brfc::rdb;

    TablePtr t1 = Table::create("t1");
    TablePtr t2 = Table::create("t2");
    AliasPtr t2a = t2->alias("t2a");
    TablePtr t3 = Table::create("t3");
    JoinPtr j = t1->join(t2a, xpr.integer(1))->join(t3, xpr.integer(1));
    EXPECT_TRUE(j->contains(t1));
    EXPECT_TRUE(not j->contains(t2));
    EXPECT_TRUE(j->contains(t2a));
    EXPECT_TRUE(j->contains(t3));
}

} // namespace expr
} // namespace brfc
