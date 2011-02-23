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
#include <brfc/expr/MockExpressionList.hpp>

using ::testing::Ref;
using ::testing::Return;

namespace brfc {
namespace expr {

class expr_Expression_test : public ::testing::Test {
  public:
    expr_Expression_test()
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

TEST_F(expr_Expression_test, test_add) {
    BinaryOperatorPtr op = xpr1.add(xpr2);

    EXPECT_EQ(BinaryOperator::ADD, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(xpr2));
}

TEST_F(expr_Expression_test, test_sub) {
    BinaryOperatorPtr op = xpr1.sub(xpr2);

    EXPECT_EQ(BinaryOperator::SUB, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(xpr2));
}

TEST_F(expr_Expression_test, test_mul) {
    BinaryOperatorPtr op = xpr1.mul(xpr2);

    EXPECT_EQ(BinaryOperator::MUL, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(xpr2));
}

TEST_F(expr_Expression_test, test_div) {
    BinaryOperatorPtr op = xpr1.div(xpr2);

    EXPECT_EQ(BinaryOperator::DIV, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(xpr2));
}

TEST_F(expr_Expression_test, test_in) {
    MockExpressionList el;
    ExpressionListPtr elp (&el, no_delete);
    ON_CALL(el, clone())
        .WillByDefault(Return(elp));
    ON_CALL(el, do_equals(Ref(el)))
        .WillByDefault(Return(true));

    BinaryOperatorPtr op = xpr1.in(el);
    EXPECT_EQ(BinaryOperator::IN, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(el));
}

TEST_F(expr_Expression_test, test_not_in) {
    MockExpressionList el;
    ExpressionListPtr elp (&el, no_delete);
    ON_CALL(el, clone())
        .WillByDefault(Return(elp));
    ON_CALL(el, do_equals(Ref(el)))
        .WillByDefault(Return(true));

    BinaryOperatorPtr op = xpr1.not_in(el);
    EXPECT_EQ(BinaryOperator::NOT_IN, op->op());
    EXPECT_TRUE(op->lhs()->equals(xpr1));
    EXPECT_TRUE(op->rhs()->equals(el));
}

} // namespace expr
} // namespace brfc
