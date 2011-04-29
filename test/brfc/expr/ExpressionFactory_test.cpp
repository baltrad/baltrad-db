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

#include <brfc/exceptions.hpp>


#include <brfc/expr/Expression.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace expr {

class expr_ExpressionFactory_test : public ::testing::Test {
  public:
    expr_ExpressionFactory_test()
            : xpr() {
    }

    ExpressionFactory xpr;
};

TEST_F(expr_ExpressionFactory_test, test_combined_datetime) {
    Expression x;
    EXPECT_NO_THROW(x = xpr.combined_datetime("what/date", "what/time"));
    
    Expression e = xpr.add(xpr.attribute("what/date"), xpr.attribute("what/time"));
    EXPECT_EQ(e, x);
}

TEST_F(expr_ExpressionFactory_test, test_and_list) {
    Expression exprs;
    exprs.push_back(xpr.long_(1));
    exprs.push_back(xpr.long_(2));
    exprs.push_back(xpr.long_(3));
    
    Expression expected = xpr.and_(xpr.and_(xpr.long_(1), xpr.long_(2)), xpr.long_(3));

    EXPECT_EQ(expected, xpr.and_(exprs));
}

TEST_F(expr_ExpressionFactory_test, test_and_list_empty) {
    EXPECT_THROW(xpr.and_(Expression()), value_error);
}

TEST_F(expr_ExpressionFactory_test, test_and_list_single) {
    Expression exprs;
    exprs.push_back(xpr.long_(1));

    EXPECT_EQ(xpr.long_(1), xpr.and_(exprs));
}

TEST_F(expr_ExpressionFactory_test, test_or_list) {
    Expression exprs;
    exprs.push_back(xpr.long_(1));
    exprs.push_back(xpr.long_(2));
    exprs.push_back(xpr.long_(3));

    Expression expected = xpr.or_(xpr.or_(xpr.long_(1), xpr.long_(2)), xpr.long_(3));
    ASSERT_TRUE(exprs.is_list());
    EXPECT_EQ(expected, xpr.or_(exprs));
}

TEST_F(expr_ExpressionFactory_test, test_or_list_empty) {
    EXPECT_THROW(xpr.or_(Expression()), value_error);
}

TEST_F(expr_ExpressionFactory_test, test_or_list_single) {
    Expression exprs;
    exprs.push_back(xpr.long_(1));

    EXPECT_EQ(xpr.long_(1), xpr.or_(exprs));
}

TEST_F(expr_ExpressionFactory_test, test_not) {
    Expression x = Listcons().symbol("not").bool_(true).get();
    EXPECT_EQ(x, xpr.not_(xpr.bool_(true)));
}

} // namespace expr
} // namespace brfc
