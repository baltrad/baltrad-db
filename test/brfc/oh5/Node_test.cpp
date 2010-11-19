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

#include <vector>

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Node.hpp>
#include <brfc/oh5/NodeBackend.hpp>

#include <brfc/test_common.hpp>
#include <brfc/oh5/MockNode.hpp>

using ::testing::_;
using ::testing::Return;

namespace brfc {
namespace oh5 {

class FakeNodeBackend : public NodeBackend {
  public:
    FakeNodeBackend(Node* front=0)
            : NodeBackend(front)
            , children_() {
    }

  protected:
    virtual std::vector<Node*> do_children() {
        return children_;
    }

    virtual std::vector<const Node*> do_children() const {
        std::vector<const Node*> vec(children_.begin(), children_.end());
        return vec;
    }

    virtual Node& do_create_child(Node* node) {
        children_.push_back(node);
        return *node;
    }
  
  private:
    std::vector<Node*> children_;
};

class FakeNode : public Node {
  public:
    FakeNode(const String& name)
            : Node(name) {
        backend(new FakeNodeBackend());
    }
  
  protected:
    virtual bool do_accepts_child(const Node&) const { return true; }
};

struct oh5_Node_test : public ::testing::Test {
    oh5_Node_test()
            : a("a")
            , f("f")
            , b(a.create_child(new FakeNode("b")))
            , c(b.create_child(new FakeNode("c"))) {
    }

    FakeNode a, f;
    Node& b, &c;
};

TEST_F(oh5_Node_test, test_has_child) {
    EXPECT_TRUE(a.has_child("b"));
    EXPECT_TRUE(a.has_child("b/c"));
    EXPECT_FALSE(a.has_child("c"));
}

TEST_F(oh5_Node_test, test_child) {
    EXPECT_EQ(&b, a.child("b"));
    EXPECT_EQ(0,  a.child("c"));
    EXPECT_EQ(&c, a.child("b/c"));
    EXPECT_EQ(0,  a.child("b/q"));
}

TEST_F(oh5_Node_test, test_has_child_absolute_path_throws) {
    EXPECT_NO_THROW(a.has_child("/b")); // can access by absolute path on root
    EXPECT_THROW(b.has_child("/c"), value_error);
}

TEST_F(oh5_Node_test, test_parent) {
    EXPECT_EQ(0, a.parent());
    EXPECT_EQ(&a, b.parent());
}

TEST_F(oh5_Node_test, path) {
    EXPECT_EQ("/a", a.path());
    EXPECT_EQ("/a/b/c", c.path());

    MockNode root("");
    EXPECT_EQ("/", root.path());
}

TEST_F(oh5_Node_test, test_root) {
    EXPECT_EQ(&a, &c.root());
    EXPECT_EQ(&a, &b.root());
    EXPECT_EQ(&a, &a.root());
    EXPECT_EQ(&f, &f.root());
}

TEST_F(oh5_Node_test, test_is_root) {
    EXPECT_TRUE(a.is_root());
    EXPECT_FALSE(b.is_root());
}

TEST_F(oh5_Node_test, test_iterator) {
    Node::iterator i = a.begin();
    EXPECT_EQ(&a, &(*i));
    ++i;
    EXPECT_EQ(&b, &(*i));
}

TEST_F(oh5_Node_test, test_iterator_end) {
    Node::iterator i = f.begin();
    EXPECT_TRUE(i != f.end());
    ++i;
    EXPECT_TRUE(i == f.end());
}

TEST_F(oh5_Node_test, test_iterate_tree) {
    a.create_child(new FakeNode("f"));

    StringList names;
    BOOST_FOREACH(Node& node, a) {
        names.append(node.name());
    }

    EXPECT_EQ(names.join(""), "abfc");
}

} // namespace oh5
} // namespace brfc
