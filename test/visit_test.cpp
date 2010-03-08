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
