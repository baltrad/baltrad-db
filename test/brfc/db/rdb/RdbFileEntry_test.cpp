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
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/oh5/Group.hpp>

#include "../../sql/MockConnection.hpp"

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_RdbFileEntry_test : public ::testing::Test {
  public:
    db_rdb_RdbFileEntry_test()
            : db(make_shared<sql::MockConnection>()) {
    
    }


    RelationalDatabase db;
};

TEST_F(db_rdb_RdbFileEntry_test, test_ctor) {
    RdbFileEntry e(&db);
    
    EXPECT_EQ(&e, e.root().file());
    EXPECT_EQ(0, e.root().parent());
}

} // namespace rdb
} // namespace db
} // namespace brfc
