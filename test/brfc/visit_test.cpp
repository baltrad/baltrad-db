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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <brfc/visit.hpp>

namespace brfc {

namespace {

class Base {
  public:
    virtual ~Base() {}
};

class A : public Base { };
class B : public Base { };
class C : public A { };

class VisitorA {
  public:
    typedef mpl::vector<A> accepted_types;

    void operator()(const A& node) { }
};

class VisitorAC {
  public:
    typedef mpl::vector<A, C> accepted_types;

    VisitorAC()
            : a(false)
            , c(false) {
    }

    void operator()(const A& node) { a = true; }
    void operator()(const C& node) { c = true; }

    bool a, c;
};

} // namespace anonymous

TEST(visit_test, type_acceptance) {
    A a;
    B b;
    VisitorA v;
    EXPECT_TRUE(visit(a, v));
    EXPECT_FALSE(visit(b, v));
}

TEST(visit_test, stop_on_first) {
  C c;
  VisitorAC v;
  
  EXPECT_TRUE(visit(c, v));
  EXPECT_TRUE(v.a);
  EXPECT_FALSE(v.c);
}

} // namespace brfc
