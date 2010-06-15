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

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include "../common.hpp"

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

class FakeNode : public Node {
  public:
    FakeNode(const String& name)
            : Node(name) {
    }
  
  protected:
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    virtual bool do_accepts_child(const Node& node) const {
        return true;
    }

    virtual bool do_accepts_parent(const Node& node) const {
        return true;
    }
};

struct oh5_Node_test : public ::testing::Test {
    oh5_Node_test()
            : a(make_shared<FakeNode>("a"))
            , b(make_shared<FakeNode>("b"))
            , c(make_shared<FakeNode>("c"))
            , f(make_shared<FakeNode>("f")) {
        a->add_child(b);
        b->add_child(c);
    }

    shared_ptr<FakeNode> a, b, c, f;
};

TEST_F(oh5_Node_test, test_has_child) {
    EXPECT_TRUE(a->has_child(*b));
}

TEST_F(oh5_Node_test, test_has_child_by_name) {
    EXPECT_TRUE(a->has_child_by_name("b"));
}

TEST_F(oh5_Node_test, test_add_child) {
    a->add_child(f);
    EXPECT_TRUE(a->has_child(*f));
    EXPECT_EQ(f->parent(), a);
}

TEST_F(oh5_Node_test, remove_child) {
    a->remove_child(*b);
    EXPECT_FALSE(a->has_child(*b));
    EXPECT_TRUE(b->is_root());
}

TEST_F(oh5_Node_test, test_add_child_null_pointer) {
    EXPECT_THROW(a->add_child(shared_ptr<Node>()), value_error);
}

TEST_F(oh5_Node_test, test_parent) {
    EXPECT_EQ(b->parent(), a);
}

TEST_F(oh5_Node_test, test_reparent) {
    a->add_child(c);
    EXPECT_TRUE(a->has_child(*c));
    EXPECT_EQ(c->parent(), a);
    EXPECT_FALSE(b->has_child(*c));
}

TEST_F(oh5_Node_test, path) {
    EXPECT_EQ(a->path(), "/a");
    EXPECT_EQ(c->path(), "/a/b/c");
}

TEST_F(oh5_Node_test, throws_on_duplicate_child) {
    EXPECT_THROW(a->add_child(b), duplicate_entry);
}

TEST_F(oh5_Node_test, changing_child_name_throws_when_parent_has_entry) {
    a->add_child(f);
    EXPECT_THROW(f->name("b"), duplicate_entry);
}

TEST_F(oh5_Node_test, root) {
    EXPECT_EQ(a, c->root());
    EXPECT_EQ(a, b->root());
    EXPECT_EQ(a, a->root());
    EXPECT_EQ(f, f->root());
}

TEST_F(oh5_Node_test, is_root) {
    EXPECT_TRUE(a->is_root());
    EXPECT_FALSE(b->is_root());
}

TEST_F(oh5_Node_test, iterator) {
    Node::iterator i = a->begin();
    EXPECT_EQ(&(*i), a.get());
    ++i;
    EXPECT_EQ(&(*i), b.get());
}

TEST_F(oh5_Node_test, iterator_end) {
    Node::iterator i = f->begin();
    EXPECT_TRUE(i != f->end());
    ++i;
    EXPECT_TRUE(i == f->end());
}

TEST_F(oh5_Node_test, iterate_tree) {
    a->add_child(f);

    StringList names;
    BOOST_FOREACH(Node& node, *a) {
        names.append(node.name());
    }

    EXPECT_EQ(names.join(""), "abfc");
}

TEST_F(oh5_Node_test, child_by_name) {
    EXPECT_EQ(a->child_by_name("b"), b);
    EXPECT_FALSE(a->child_by_name("c"));
}

TEST_F(oh5_Node_test, child_by_path) {
    EXPECT_EQ(a->child_by_path("b/c"), c);
    EXPECT_EQ(a->child_by_path("b"), b);
    EXPECT_FALSE(a->child_by_path("b/q"));
}

} // namespace oh5
} // namespace brfc
