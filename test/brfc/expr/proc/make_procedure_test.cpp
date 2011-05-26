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

#include <stdexcept>

#include <gtest/gtest.h>

#include <brfc/expr/Listcons.hpp>
#include <brfc/expr/proc/make_procedure.hpp>

namespace brfc {
namespace proc {

TEST(expr_proc_make_procedure_test, test_call) {
    Expression in = Listcons().int64(1).int64(2).get();
    Expression out = Listcons().symbol("proc").int64(1).int64(2).get();
    
    EXPECT_EQ(out, make_procedure("proc", 2)(in));
}

TEST(expr_proc_make_procedure_test, test_call_less_args) {
    Expression in = Listcons().int64(1).get();
    
    EXPECT_THROW(make_procedure("proc", 2)(in), std::logic_error);
}

TEST(expr_proc_make_procedure_test, test_call_more_args) {
    Expression in = Listcons().int64(1).int64(2).int64(3).get();
    
    EXPECT_THROW(make_procedure("proc", 2)(in), std::logic_error);
}

TEST(expr_proc_make_procedure_test, test_call_any) {
    Expression in1 = Listcons().int64(1).get();
    Expression out1 = Listcons().symbol("proc").int64(1).get();
    Expression in2 = Listcons().int64(1).int64(2).get();
    Expression out2 = Listcons().symbol("proc").int64(1).int64(2).get();

    make_procedure proc("proc");
    EXPECT_EQ(out1, proc(in1));
    EXPECT_EQ(out2, proc(in2));
}

}
}
