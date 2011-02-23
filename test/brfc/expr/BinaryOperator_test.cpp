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

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/MockExpression.hpp>

namespace brfc {
namespace expr {

using ::testing::Ref;
using ::testing::Return;

class expr_BinaryOperator_test : public ::testing::Test {
  public:
    expr_BinaryOperator_test()
            : xpr1()
            , xpr2()
            , xpr1p(&xpr1, no_delete)
            , xpr2p(&xpr2, no_delete) {

    }

    virtual void SetUp() {
        ON_CALL(xpr1, clone())
            .WillByDefault(Return(xpr1p));
        ON_CALL(xpr1, do_equals(Ref(xpr1)))
            .WillByDefault(Return(true));
        ON_CALL(xpr2, clone())
            .WillByDefault(Return(xpr2p));
        ON_CALL(xpr2, do_equals(Ref(xpr2)))
            .WillByDefault(Return(true));
    }

    ::testing::NiceMock<MockExpression> xpr1;
    ::testing::NiceMock<MockExpression> xpr2;
    ExpressionPtr xpr1p, xpr2p;
};

TEST_F(expr_BinaryOperator_test, test_equals) {
    BinaryOperator op1(BinaryOperator::ADD, xpr1, xpr2);
    BinaryOperator op2(BinaryOperator::ADD, xpr2, xpr1);
    BinaryOperator op3(BinaryOperator::MUL, xpr1, xpr2);

    EXPECT_TRUE(op1.equals(op1));
    EXPECT_TRUE(op1.equals(op2));
    EXPECT_FALSE(op1.equals(op3));
    EXPECT_TRUE(op2.equals(op1));
    EXPECT_TRUE(op2.equals(op2));
    EXPECT_FALSE(op2.equals(op3));
    EXPECT_FALSE(op3.equals(op1));
    EXPECT_FALSE(op3.equals(op2));
    EXPECT_TRUE(op3.equals(op3));
}

} // namespace expr
} // namespace brfc
