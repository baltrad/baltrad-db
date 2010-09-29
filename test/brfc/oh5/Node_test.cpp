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

#include <brfc/oh5/Attribute.hpp>
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

TEST_F(oh5_Node_test, test_construct_with_path) {
    EXPECT_THROW(FakeNode("a/b"), value_error);
}

TEST_F(oh5_Node_test, test_has_child) {
    EXPECT_TRUE(a->has_child("b"));
    EXPECT_TRUE(a->has_child("b/c"));
    EXPECT_FALSE(a->has_child("c"));
}

TEST_F(oh5_Node_test, test_child) {
    EXPECT_EQ(b, a->child("b"));
    EXPECT_EQ(0, a->child("c"));
    EXPECT_EQ(c, a->child("b/c"));
    EXPECT_EQ(0, a->child("b/q"));
}

TEST_F(oh5_Node_test, test_has_child_absolute_path_throws) {
    EXPECT_NO_THROW(a->has_child("/b")); // can access by absolute path on root
    EXPECT_THROW(b->has_child("/c"), value_error);
}

TEST_F(oh5_Node_test, test_children) {
    std::vector<Node*> nodes = a->children();

    ASSERT_EQ(1, nodes.size());
    EXPECT_EQ(b, nodes.at(0));
}

TEST_F(oh5_Node_test, test_add_child) {
    Node& f = a->add_child(f_);
    EXPECT_TRUE(a->has_child(f.name()));
    EXPECT_EQ(f.parent(), a);
}

TEST_F(oh5_Node_test, test_add_child_null_pointer_throws) {
    EXPECT_THROW(a->add_child(auto_ptr<Node>()), value_error);
}

TEST_F(oh5_Node_test, test_parent) {
    EXPECT_EQ(b->parent(), a);
}

TEST_F(oh5_Node_test, path) {
    EXPECT_EQ(a->path(), "/a");
    EXPECT_EQ(c->path(), "/a/b/c");
}

TEST_F(oh5_Node_test, test_add_child_duplicate_throws) {
    auto_ptr<Node> b2(new FakeNode("b"));
    EXPECT_THROW(a->add_child(b2), duplicate_entry);
}

TEST_F(oh5_Node_test, test_root) {
    EXPECT_EQ(a, &c->root());
    EXPECT_EQ(a, &b->root());
    EXPECT_EQ(a, &a->root());
    EXPECT_EQ(f_.get(), &f_->root());
}

TEST_F(oh5_Node_test, test_is_root) {
    EXPECT_TRUE(a->is_root());
    EXPECT_FALSE(b->is_root());
}

TEST_F(oh5_Node_test, test_iterator) {
    Node::iterator i = a->begin();
    EXPECT_EQ(&(*i), a);
    ++i;
    EXPECT_EQ(&(*i), b);
}

TEST_F(oh5_Node_test, test_iterator_end) {
    Node::iterator i = f_->begin();
    EXPECT_TRUE(i != f_->end());
    ++i;
    EXPECT_TRUE(i == f_->end());
}

TEST_F(oh5_Node_test, test_iterate_tree) {
    a->add_child(f_);

    StringList names;
    BOOST_FOREACH(Node& node, *a) {
        names.append(node.name());
    }

    EXPECT_EQ(names.join(""), "abfc");
}

TEST_F(oh5_Node_test, test_create_attribute_invalid) {
    Scalar val(1);
    EXPECT_THROW(a->create_attribute("", val), value_error);
    EXPECT_THROW(a->create_attribute("/", val), value_error);
    EXPECT_THROW(b->create_attribute("/asdasd", val), value_error);
}

TEST_F(oh5_Node_test, test_create_attribute) {
    Scalar val(1);
    EXPECT_NO_THROW(a->create_attribute("/qwe", val));
    EXPECT_TRUE(a->has_child("qwe"));

    EXPECT_THROW(a->create_attribute("qwe", val), duplicate_entry);
}

TEST_F(oh5_Node_test, test_create_attribute_path) {
    Scalar val(1);
    EXPECT_NO_THROW(a->create_attribute("path/to/attr", val));
    EXPECT_TRUE(a->has_child("path/to/attr"));
    
    // 'path' is duplicate
    EXPECT_THROW(a->create_attribute("path/another/attr", val), duplicate_entry);

    // 'another' not created
    EXPECT_FALSE(a->has_child("path/another"));

}

TEST_F(oh5_Node_test, test_create_group_invalid) {
    EXPECT_THROW(a->create_group(""), value_error);
    EXPECT_THROW(a->create_group("/"), value_error);
    EXPECT_THROW(b->create_group("/asdasd"), value_error);
}

TEST_F(oh5_Node_test, test_create_group) {
    EXPECT_NO_THROW(a->create_group("/qwe"));
    EXPECT_TRUE(a->has_child("qwe"));

    EXPECT_THROW(a->create_group("qwe"), duplicate_entry);
}

TEST_F(oh5_Node_test, test_create_group_path) {
    EXPECT_NO_THROW(a->create_group("path/to/group"));
    EXPECT_TRUE(a->has_child("path/to/group"));
    
    // 'path' is duplicate
    EXPECT_THROW(a->create_group("path/another/group"), duplicate_entry);

    // 'another' not created
    EXPECT_FALSE(a->has_child("path/another"));

}

} // namespace oh5
} // namespace brfc
