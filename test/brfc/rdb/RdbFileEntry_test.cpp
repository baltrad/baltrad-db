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

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/sql/MockConnection.hpp>
#include <brfc/sql/MockConnectionPool.hpp>
#include <brfc/util/no_delete.hpp>

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

namespace brfc {

class rdb_RdbFileEntry_test : public ::testing::Test {
  public:
    rdb_RdbFileEntry_test()
            : conn()
            , pool()
            , pool_ptr(&pool, no_delete)
            , db(pool_ptr)
            , entry(&db) {
    
    }

    virtual void SetUp() {
        ON_CALL(pool, do_get())
            .WillByDefault(Return(&conn));
    }
    
    sql::MockConnection conn;
    sql::MockConnectionPool pool;
    boost::shared_ptr<sql::ConnectionPool> pool_ptr;
    RelationalDatabase db;
    RdbFileEntry entry;
};

TEST_F(rdb_RdbFileEntry_test, test_ctor) {
    EXPECT_EQ(0, entry.metadata().root().parent());
}

} // namespace brfc
