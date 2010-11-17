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

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RdbFileEntry_test : public ::testing::Test {
  public:
    db_rdb_RdbFileEntry_test()
            : conn()
            , db(shared_ptr<sql::Connection>(&conn, no_delete))
            , entry(&db) {
    
    }

    sql::MockConnection conn;
    RelationalDatabase db;
    RdbFileEntry entry;
};

/*
TEST_F(db_rdb_RdbFileEntry_test, test_ctor) {
    EXPECT_EQ(&entry, entry.root().file());
    EXPECT_EQ(0, entry.root().parent());
}

TEST_F(db_rdb_RdbFileEntry_test, test_source_id) {
    EXPECT_CALL(helper, load_file(Ref(entry)));
    entry.source_id();

    entry.source_id(10);
    entry.source_id();
}

TEST_F(db_rdb_RdbFileEntry_test, test_source) {
    entry.source_id(10);

    oh5::Source src;
    src.add("name", "test");

    EXPECT_CALL(helper, select_source(10))
        .WillOnce(Return(src));

    entry.source();
    entry.source();
}

TEST_F(db_rdb_RdbFileEntry_test, test_lo_id) {
    EXPECT_CALL(helper, load_file(Ref(entry)));
    entry.lo_id();

    entry.lo_id(10);
    entry.lo_id();
}
*/
} // namespace rdb
} // namespace db
} // namespace brfc
