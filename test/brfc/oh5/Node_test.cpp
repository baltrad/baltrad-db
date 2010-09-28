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
            : Node(0, name) {
    }
  
  protected:
    virtual bool do_accepts_child(const Node& node) const {
        return true;
    }

    virtual bool do_accepts_parent(const Node& node) const {
        return true;
    }
};

struct oh5_Node_test : public ::testing::Test {
    oh5_Node_test()
            : a_(new FakeNode("a"))
            , b_(new FakeNode("b"))
            , c_(new FakeNode("c"))
            , f_(new FakeNode("f")) {
        a = a_.get();
        c = &b_->add_child(c_);
        b = &a_->add_child(b_);
    }

    auto_ptr<Node> a_, b_, c_, f_;
    Node* a, *b, *c;
};

TEST_F(oh5_Node_test, test_has_child) {
    EXPECT_TRUE(a->has_child(*b));
}

TEST_F(oh5_Node_test, test_has_child_by_name) {
    EXPECT_TRUE(a->has_child_by_name("b"));
}

TEST_F(oh5_Node_test, test_children) {
    std::vector<Node*> nodes = a->children();

    ASSERT_EQ(1, nodes.size());
    EXPECT_EQ(b, nodes.at(0));
}

TEST_F(oh5_Node_test, test_add_child) {
    Node& f = a->add_child(f_);
    EXPECT_TRUE(a->has_child(f));
    EXPECT_EQ(f.parent(), a);
}

TEST_F(oh5_Node_test, test_add_child_null_pointer) {
    EXPECT_THROW(a->add_child(auto_ptr<Node>()), value_error);
}

TEST_F(oh5_Node_test, test_parent) {
    EXPECT_EQ(b->parent(), a);
}

TEST_F(oh5_Node_test, path) {
    EXPECT_EQ(a->path(), "/a");
    EXPECT_EQ(c->path(), "/a/b/c");
}

TEST_F(oh5_Node_test, throws_on_duplicate_child) {
    auto_ptr<Node> b2(new FakeNode("b"));
    EXPECT_THROW(a->add_child(b2), duplicate_entry);
}

TEST_F(oh5_Node_test, root) {
    EXPECT_EQ(a, &c->root());
    EXPECT_EQ(a, &b->root());
    EXPECT_EQ(a, &a->root());
    EXPECT_EQ(f_.get(), &f_->root());
}

TEST_F(oh5_Node_test, is_root) {
    EXPECT_TRUE(a->is_root());
    EXPECT_FALSE(b->is_root());
}

TEST_F(oh5_Node_test, iterator) {
    Node::iterator i = a->begin();
    EXPECT_EQ(&(*i), a);
    ++i;
    EXPECT_EQ(&(*i), b);
}

TEST_F(oh5_Node_test, iterator_end) {
    Node::iterator i = f_->begin();
    EXPECT_TRUE(i != f_->end());
    ++i;
    EXPECT_TRUE(i == f_->end());
}

TEST_F(oh5_Node_test, iterate_tree) {
    a->add_child(f_);

    StringList names;
    BOOST_FOREACH(Node& node, *a) {
        names.append(node.name());
    }

    EXPECT_EQ(names.join(""), "abfc");
}

TEST_F(oh5_Node_test, child_by_name) {
    EXPECT_EQ(b, a->child_by_name("b"));
    EXPECT_EQ(0, a->child_by_name("c"));
}

TEST_F(oh5_Node_test, child_by_path) {
    EXPECT_EQ(c, a->child_by_path("b/c"));
    EXPECT_EQ(b, a->child_by_path("b"));
    EXPECT_FALSE(a->child_by_path("b/q"));
}

} // namespace oh5
} // namespace brfc
