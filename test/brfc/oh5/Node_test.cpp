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
#include <brfc/oh5/NodeImpl.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

class FakeNodeImpl;

class FakeNode : public Node {
  public:
    FakeNode(const String& name);

    void add_child(FakeNode* node);
  
  protected:
    virtual bool do_accepts_child(const Node& node) const {
        return true;
    }
};

class FakeNodeImpl : public NodeImpl {
  public:
    FakeNodeImpl(const String& name)
            : name_(name)
            , children_()
            , parent_(0) {

    }

    void parent(FakeNode* node) { parent_ = node; }

  protected:
    virtual Group* do_create_group(const String& path) {
        throw std::runtime_error("not implemented");
    }

    virtual Attribute* do_create_attribute(const String& path,
                                           const Scalar& value) {
        throw std::runtime_error("not implemented");
    }

    virtual const String& do_name() const { return name_; }

    virtual Node* do_parent() {
        return parent_;
    }

    virtual const Node* do_parent() const {
        return parent_;
    }

    virtual std::vector<Node*> do_children() {
        return children_;
    }

    virtual std::vector<const Node*> do_children() const {
        std::vector<const Node*> vec(children_.begin(), children_.end());
        return vec;
    }

    virtual Node& do_add_child(Node* node) {
        children_.push_back(node);
        return *node;
    }
  
  private:
    String name_;
    std::vector<Node*> children_;
    Node* parent_;
};

FakeNode::FakeNode(const String& name)
        : Node(new FakeNodeImpl(name)) {
}

void
FakeNode::add_child(FakeNode* node) {
    static_cast<FakeNodeImpl&>(impl()).add_child(node);
    static_cast<FakeNodeImpl&>(node->impl()).parent(this);
}


struct oh5_Node_test : public ::testing::Test {
    oh5_Node_test()
            : a("a")
            , b("b")
            , c("c")
            , f("f") {
        b.add_child(&c);
        a.add_child(&b);
    }
    
    FakeNode a, b, c, f;

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

    FakeNode root("");
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
    a.add_child(&f);

    StringList names;
    BOOST_FOREACH(Node& node, a) {
        names.append(node.name());
    }

    EXPECT_EQ(names.join(""), "abfc");
}

} // namespace oh5
} // namespace brfc
