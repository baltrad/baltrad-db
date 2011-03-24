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

#include <brfc/exceptions.hpp>

#include <brfc/oh5/MockNodeBackend.hpp>
#include <brfc/oh5/MockNode.hpp>

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnNull;
using ::testing::ReturnRef;

namespace brfc {
namespace oh5 {

struct oh5_NodeBackend_test : public ::testing::Test {
    oh5_NodeBackend_test()
             : root("")
             , backend(root) {
    }

    MockNode root;
    MockNodeBackend backend;
};

TEST_F(oh5_NodeBackend_test, test_add) {
    MockNode n("n");
    EXPECT_CALL(backend, do_add(&n))
        .WillOnce(ReturnRef(n));

    Node& rn = backend.add(root, &n);
    EXPECT_EQ(&n, &rn);
    EXPECT_EQ(&backend, &rn.backend());
    EXPECT_EQ(&root, rn.parent());
}

TEST_F(oh5_NodeBackend_test, test_add_duplicate) {
    MockNode n("n");
    EXPECT_CALL(backend, do_add(&n))
        .WillOnce(ReturnRef(n));
    
    backend.add(root, &n);
    EXPECT_THROW(backend.add(root, &n), value_error);
}

TEST_F(oh5_NodeBackend_test, test_add_invalid_parent) {
    MockNode r2("n");
    MockNodeBackend be2(r2);

    MockNode* n = new MockNode("n2");

    ON_CALL(*n, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    EXPECT_THROW(backend.add(r2, n), value_error); 
}

TEST_F(oh5_NodeBackend_test, test_add_null) {
    EXPECT_THROW(backend.add(root, 0), value_error);
}

TEST_F(oh5_NodeBackend_test, test_node) {
    MockNode n1("n1"), n2("n2"), n3("n3");
    std::vector<const Node*> c1, c2;
    c1.push_back(&n1);
    c1.push_back(&n2);
    c2.push_back(&n3);
    
    EXPECT_CALL(backend, do_root())
        .WillOnce(ReturnRef(root));
    EXPECT_CALL(backend, do_children(Ref(root)))
        .WillOnce(Return(c1));
    EXPECT_CALL(backend, do_children(Ref(n1)))
        .WillOnce(Return(c2));
    
    EXPECT_EQ(&n3, backend.node("/n1/n3"));
}

TEST_F(oh5_NodeBackend_test, test_node_root) {
    EXPECT_CALL(backend, do_root())
        .WillOnce(ReturnRef(root));
    
    EXPECT_EQ(&root, backend.node("/"));
}

TEST_F(oh5_NodeBackend_test, test_node_missing) {
    std::vector<const Node*> c1; 
    EXPECT_CALL(backend, do_root())
        .WillOnce(ReturnRef(root));
    EXPECT_CALL(backend, do_children(Ref(root)))
        .WillOnce(Return(c1));

    EXPECT_FALSE(backend.node("foo"));
}

} // namespace oh5
} // namespace brfc
