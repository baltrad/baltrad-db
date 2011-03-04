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

#include <brfc/exceptions.hpp>

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>

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
    ExpressionPtr x;
    EXPECT_NO_THROW(x = xpr.combined_datetime("what/date", "what/time"));

    BinaryOperatorPtr op = dynamic_pointer_cast<BinaryOperator>(x);
    ASSERT_TRUE(op);

    EXPECT_EQ(BinaryOperator::ADD, op->type());

    AttributePtr p = dynamic_pointer_cast<Attribute>(op->lhs());
    ASSERT_TRUE(p);
    EXPECT_EQ("what/date", p->name());

    p = dynamic_pointer_cast<Attribute>(op->rhs());
    ASSERT_TRUE(p);
    EXPECT_EQ("what/time", p->name());
}

TEST_F(expr_ExpressionFactory_test, test_combined_datetime_invalid) {
    ExpressionPtr x;
    EXPECT_THROW(x = xpr.combined_datetime("what/time", "what/date"),
                 value_error);
    
    EXPECT_THROW(x = xpr.combined_datetime("what/date", "what/object"),
                 value_error);

    EXPECT_THROW(x = xpr.combined_datetime("what/object", "what/time"),
                 value_error);

    EXPECT_THROW(x = xpr.combined_datetime("what/object", "what/object"),
                 value_error);
}

} // namespace expr
} // namespace brfc
