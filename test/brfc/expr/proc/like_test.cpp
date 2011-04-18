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

#include <gtest/gtest.h>

#include <stdexcept>

#include <brfc/test_common.hpp>
#include <brfc/expr/listcons.hpp>
#include <brfc/expr/proc/like.hpp>

namespace brfc {
namespace expr {
namespace proc {

TEST(expr_proc_like_test, test_sexp_dispatch) {
    EXPECT_EQ(sexp(false), like()(listcons().string("foo").string("bar").get()));
}

TEST(expr_proc_like_test, test_invalid_types) {
    EXPECT_THROW(like()(1, 2), std::logic_error);
    EXPECT_THROW(like()(std::string("str"), 2), std::logic_error);
    EXPECT_THROW(like()(2, std::string("pattern")), std::logic_error);
}

TEST(expr_proc_like_test, test_plain) {
    EXPECT_EQ(sexp(true), like()(std::string("foo"), std::string("foo")));
    EXPECT_EQ(sexp(false), like()(std::string("foo"), std::string("bar")));
}

TEST(expr_proc_like_test, test_star) {
    std::string s = "foobarbaz";
    EXPECT_EQ(sexp(true), like()(s, std::string("foo*")));
    EXPECT_EQ(sexp(true), like()(s, std::string("*bar*")));
    EXPECT_EQ(sexp(true), like()(s, std::string("*baz")));
    EXPECT_EQ(sexp(false), like()(s, std::string("bar*")));
    EXPECT_EQ(sexp(false), like()(s, std::string("*bab*")));
    EXPECT_EQ(sexp(false), like()(s, std::string("*bab")));
}

TEST(expr_proc_like_test, test_qmark) {
    std::string s = "foobar";
    
    EXPECT_EQ(sexp(true), like()(s, std::string("f?o??r")));
    EXPECT_EQ(sexp(false), like()(s, std::string("foob?")));
    EXPECT_EQ(sexp(false), like()(s, std::string("foobar?")));
    EXPECT_EQ(sexp(false), like()(s, std::string("?foob??")));
}

}
}
}
