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

#include <brfc/exceptions.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/PhysicalOh5File.hpp>
#include <brfc/oh5/MockNode.hpp>
#include <brfc/rdb/RdbOh5NodeBackend.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionPool.hpp>
#include <brfc/util/no_delete.hpp>

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

namespace brfc {

// XXX: test for loading unloaded nodes from DB!

class rdb_RdbOh5NodeBackend_test : public ::testing::Test {
  public:
    rdb_RdbOh5NodeBackend_test()
            : conn()
            , pool()
            , pool_ptr(&pool, no_delete)
            , db(pool_ptr)
            , node("mocknode")
            , backend(&db) {
    
    }

    virtual void SetUp() {
        ON_CALL(pool, do_get())
            .WillByDefault(Return(&conn));
    }

    sql::MockConnection conn;
    sql::MockConnectionPool pool;
    boost::shared_ptr<sql::ConnectionPool> pool_ptr;
    RelationalDatabase db;
    ::testing::NiceMock<MockNode> node;
    RdbOh5NodeBackend backend;
};

TEST_F(rdb_RdbOh5NodeBackend_test, test_ctor) {
    EXPECT_EQ(0, backend.id(backend.root()));
    EXPECT_TRUE(backend.loaded(backend.root()));
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_set_id) {
    backend.id(backend.root(), 1);
    EXPECT_EQ(1, backend.id(backend.root()));
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_set_loaded) {
    backend.loaded(backend.root(), false);
    EXPECT_FALSE(backend.loaded(backend.root()));
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_add) {
    MockNode* n = new MockNode("n");
    ON_CALL(*n, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection

    Oh5Node& rn = backend.root().add(n);

    EXPECT_EQ(n, &rn); // reuses node
    EXPECT_TRUE(backend.loaded(*n));
    EXPECT_EQ(0, backend.id(*n));
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_add_duplicate_entry) {
    MockNode* n1 = new MockNode("n");
    MockNode* n2 = new MockNode("n");
    ON_CALL(*n1, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    ON_CALL(*n2, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    
    backend.root().add(n1);
    EXPECT_THROW(backend.root().add(n2), duplicate_entry);
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_has) {
    MockNode* n = new MockNode("n");
    ON_CALL(*n, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    
    EXPECT_TRUE(backend.has(backend.root()));
    EXPECT_FALSE(backend.has(*n));
    backend.root().add(n);
    EXPECT_TRUE(backend.has(*n));
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_parent) {
    MockNode* n = new MockNode("n");
    ON_CALL(*n, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    
    backend.root().add(n);
    EXPECT_FALSE(backend.root().parent());
    EXPECT_EQ(&backend.root(), n->parent());
}

TEST_F(rdb_RdbOh5NodeBackend_test, test_children) {
    MockNode* n = new MockNode("n");
    ON_CALL(*n, do_accepts_child(_))
        .WillByDefault(Return(true)); // for leak detection
    
    std::vector<Oh5Node*> children = backend.children(backend.root());
    EXPECT_TRUE(children.empty());

    backend.root().add(n);
    children = backend.children(*n);
    EXPECT_TRUE(children.empty());
    
    children = backend.children(backend.root());
    ASSERT_EQ(1u, children.size());
    EXPECT_EQ(n, children.at(0));
}

} // namespace brfc
