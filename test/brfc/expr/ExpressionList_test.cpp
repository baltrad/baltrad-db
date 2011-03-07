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

#include <brfc/expr/ExpressionList.hpp>
#include <brfc/expr/MockExpression.hpp>

namespace brfc {
namespace expr {

using ::testing::Ref;
using ::testing::Return;

class expr_ExpressionList_test : public ::testing::Test {
  public:
    expr_ExpressionList_test()
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

TEST_F(expr_ExpressionList_test, test_append) {
    EXPECT_CALL(xpr1, clone())
        .WillOnce(Return(xpr1p));

    ExpressionList l;
    l.append(xpr1);

    EXPECT_EQ(1u, l.size());
}

TEST_F(expr_ExpressionList_test, test_clone) {
    ExpressionList l;
    l.append(xpr1);
    l.append(xpr2);

    EXPECT_CALL(xpr1, clone())
        .WillOnce(Return(xpr1p));
    EXPECT_CALL(xpr2, clone())
        .WillOnce(Return(xpr2p));

    ExpressionListPtr copy = dynamic_pointer_cast<ExpressionList>(l.clone());
    ASSERT_EQ(2u, copy->size());
    EXPECT_TRUE(copy->at(0).equals(xpr1));
    EXPECT_TRUE(copy->at(1).equals(xpr2));
}

TEST_F(expr_ExpressionList_test, test_equals) {
    ExpressionList l1;
    ExpressionList l2;
    l2.append(xpr1);
    l2.append(xpr2);
    ExpressionList l3;
    l3.append(xpr2);
    l3.append(xpr1);

    EXPECT_TRUE(l1.equals(l1));
    EXPECT_FALSE(l1.equals(l2));
    EXPECT_FALSE(l1.equals(l3));
    EXPECT_FALSE(l2.equals(l1));
    EXPECT_TRUE(l2.equals(l2));
    EXPECT_FALSE(l2.equals(l3));
    EXPECT_FALSE(l3.equals(l1));
    EXPECT_FALSE(l3.equals(l2));
    EXPECT_TRUE(l3.equals(l3));
}

} // namespace expr
} // namespace brfc
