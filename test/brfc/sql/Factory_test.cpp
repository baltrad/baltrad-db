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

#include <brfc/test_common.hpp>

#include <gtest/gtest.h>

#include <brfc/expr/Expression.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {
namespace sql {

class sql_Factory_test : public ::testing::Test {
  public:
    sql_Factory_test()
            : xpr() {
    }

    Factory xpr;
};

TEST_F(sql_Factory_test, test_quote) {
    Expression in(1);
    Expression out = xpr.quote(in);
    EXPECT_EQ(2u, out.size());
    Expression::const_iterator it = out.begin();
    EXPECT_EQ(it->symbol(), "quote");
    ++it;
    EXPECT_EQ(in, *it);
}

} // namespace sql
} // namespace brfc
