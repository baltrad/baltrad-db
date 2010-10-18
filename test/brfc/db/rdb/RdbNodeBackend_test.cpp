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

#include <gtest/gtest.h>

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/PhysicalFile.hpp>

#include "../../sql/MockConnection.hpp"
#include "../../oh5/MockNode.hpp"
#include "../../MockHasher.hpp"
#include "MockRdbHelper.hpp"

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RdbNodeBackend_test : public ::testing::Test {
  public:
    db_rdb_RdbNodeBackend_test()
            : conn()
            , hasher()
            , helper(&conn, &hasher)
            , db(shared_ptr<sql::Connection>(&conn, no_delete),
                 shared_ptr<RdbHelper>(&helper, no_delete))
            , entry(&db)
            , node("mocknode")
            , backend() {
    
    }

    virtual void SetUp() {
        ON_CALL(node, do_file())
            .WillByDefault(Return(&entry));
        backend.front(&node);
    }

    sql::MockConnection conn;
    MockHasher hasher;
    MockRdbHelper helper;
    RelationalDatabase db;
    RdbFileEntry entry;
    ::testing::NiceMock<oh5::MockNode> node;
    RdbNodeBackend backend;
};

TEST_F(db_rdb_RdbNodeBackend_test, test_children) {
    EXPECT_CALL(helper, load_children(Ref(node)));

    backend.children();
}

TEST_F(db_rdb_RdbNodeBackend_test, test_children_loaded) {
    backend.loaded(true);
    EXPECT_EQ(0, backend.children().size());
}

TEST_F(db_rdb_RdbNodeBackend_test, test_create_child) {
    oh5::Node* n = new oh5::MockNode("n");
    EXPECT_CALL(helper, insert_node(Ref(*n)));

    backend.create_child(n);
}

} // namespace rdb
} // namespace db
} // namespace brfc
