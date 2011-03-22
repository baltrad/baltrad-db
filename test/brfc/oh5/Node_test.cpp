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

#include <vector>

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/FakeNode.hpp>
#include <brfc/oh5/MockNodeBackend.hpp>

#include <brfc/test_common.hpp>

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnNull;
using ::testing::ReturnRef;

namespace brfc {
namespace oh5 {


ACTION_P(SetBackend, n) { return *arg1; }

struct oh5_Node_test : public ::testing::Test {
    oh5_Node_test()
            : r("")
            , a("a")
            , f("f") 
            , backend(r) {

    }

    virtual void SetUp() {
        ON_CALL(backend, do_has(Ref(r)))
            .WillByDefault(Return(true));
        ON_CALL(backend, do_add(_, _))
            .WillByDefault(SetBackend(&backend));
        
        backend.add(r, &a);
    }
    
    FakeNode r, a, f;
    MockNodeBackend backend;
};

TEST_F(oh5_Node_test, test_has_backend) {
    EXPECT_TRUE(a.has_backend());
    EXPECT_FALSE(f.has_backend());
}

TEST_F(oh5_Node_test, test_backend) {
    EXPECT_EQ(&backend, &a.backend());
    EXPECT_THROW(f.backend(), std::runtime_error);
}

TEST_F(oh5_Node_test, test_parent) {
    EXPECT_CALL(backend, do_parent(Ref(a)))
        .WillOnce(Return(&r));

    EXPECT_EQ(&r, a.parent());
    EXPECT_FALSE(f.parent());
}

TEST_F(oh5_Node_test, test_add) {
    EXPECT_CALL(backend, do_add(Ref(r), &f))
        .WillOnce(ReturnRef(f));
    
    EXPECT_EQ(&f, &r.add(&f));
}

TEST_F(oh5_Node_test, test_has_child) {
/*
    EXPECT_CALL(backend, do_has_child(Ref(r), Ref(a)))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(r.has_child("f"));
*/
}

TEST_F(oh5_Node_test, test_child) {

}

/*
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
*/

/*
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

    Node::iterator i = a.begin();
    EXPECT_EQ("a", (i++)->name());
    EXPECT_EQ("b", (i++)->name());
    EXPECT_EQ("f", (i++)->name());
    EXPECT_EQ("c", (i++)->name());
}
*/

} // namespace oh5
} // namespace brfc
