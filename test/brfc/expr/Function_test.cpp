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

#include <brfc/expr/Function.hpp>
#include <brfc/expr/MockExpression.hpp>

namespace brfc {
namespace expr {

using ::testing::Ref;
using ::testing::Return;

class expr_Function_test : public ::testing::Test {
  public:
    expr_Function_test()
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

TEST_F(expr_Function_test, test_equals) {
    Function f1("f1");
    f1.add_arg(xpr1);
    Function f2("f2");
    f2.add_arg(xpr1);
    Function f3("f3");
    f3.add_arg(xpr1);
    f3.add_arg(xpr2);
    Function f4("f3");
    f4.add_arg(xpr2);
    f4.add_arg(xpr1);

    EXPECT_TRUE(f1.equals(f1));
    EXPECT_FALSE(f1.equals(f2));
    EXPECT_FALSE(f1.equals(f3));
    EXPECT_FALSE(f1.equals(f4));
    EXPECT_FALSE(f2.equals(f1));
    EXPECT_TRUE(f2.equals(f2));
    EXPECT_FALSE(f2.equals(f3));
    EXPECT_FALSE(f2.equals(f4));
    EXPECT_FALSE(f3.equals(f1));
    EXPECT_FALSE(f3.equals(f2));
    EXPECT_FALSE(f3.equals(f4));
    EXPECT_FALSE(f4.equals(f1));
    EXPECT_FALSE(f4.equals(f2));
    EXPECT_FALSE(f4.equals(f3));
    EXPECT_TRUE(f4.equals(f4));
}

} // namespace expr
} // namespace brfc
