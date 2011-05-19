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

#include <gmock/gmock.h>

#include <boost/bind.hpp>

#include <brfc/expr/Eval.hpp>
#include <brfc/expr/Listcons.hpp>

using ::testing::Return;

namespace brfc {

namespace {

    struct MockProc {
        MOCK_METHOD1(call, Expression(const Expression&));
    };

} // namespace anonymous

struct expr_Eval_test : public ::testing::Test {
    expr_Eval_test()
            : proc_a()
            , proc_b()
            , proc_c()
            , eval() {
    }

    virtual void SetUp() {
        eval.bind("a", boost::bind(&MockProc::call, &proc_a, _1));
        eval.bind("b", boost::bind(&MockProc::call, &proc_b, _1));
        eval.bind("c", boost::bind(&MockProc::call, &proc_c, _1));
    }

    MockProc proc_a, proc_b, proc_c;
    Eval eval;
};

TEST_F(expr_Eval_test, test_symbol_proc) {
    EXPECT_CALL(proc_a, call(Expression()))
        .WillOnce(Return(Expression()));

    eval(Listcons().symbol("a").get());
};

TEST_F(expr_Eval_test, test_symbol_proc_return) {
    EXPECT_CALL(proc_a, call(Expression()))
        .WillOnce(Return(Expression(1)));

    EXPECT_EQ(Expression(1), eval(Listcons().symbol("a").get()));
}

TEST_F(expr_Eval_test, test_symbol_proc_lookup_failure) {
    EXPECT_THROW(eval(Listcons().symbol("q").get()), std::logic_error);
};

TEST_F(expr_Eval_test, test_proc_call_order) {
    EXPECT_CALL(proc_b, call(Listcons().int64(1).get()))
        .WillOnce(Return(Expression(10)));
    EXPECT_CALL(proc_c, call(Listcons().int64(2).get()))
        .WillOnce(Return(Expression(20)));
    EXPECT_CALL(proc_a, call(Listcons().int64(10).int64(20).get()))
        .WillOnce(Return(Expression(30)));
    
    Expression e = Listcons().symbol("a")
                             .append(Listcons().symbol("b").int64(1).get())
                             .append(Listcons().symbol("c").int64(2).get())
                             .get();
    EXPECT_EQ(Expression(30), eval(e));
};

TEST_F(expr_Eval_test, test_proc_literal_proc) {
    MockProc p;

    eval.bind_literal_proc(boost::bind(&MockProc::call, &p, _1));

    EXPECT_CALL(p, call(Expression(1)))
        .WillOnce(Return(Expression(10)));
    
    EXPECT_EQ(Expression(10), eval(Expression(1)));
}

TEST_F(expr_Eval_test, test_proc_special_proc) {
    MockProc p;

    eval.bind_special_proc("s", boost::bind(&MockProc::call, &p, _1));

    Expression in = Listcons().symbol("s")
                              .append(Listcons().symbol("b").int64(1).get())
                              .get();
    
    EXPECT_CALL(p, call(Listcons().append(Listcons().symbol("b")
                                                    .int64(1)
                                                    .get())
                                  .get()))
        .WillOnce(Return(Expression(10)));
    
    EXPECT_EQ(Expression(10), eval(in));
}

} // namespace brfc
