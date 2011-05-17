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

#include <brfc/db/MockDatabase.hpp>
#include <brfc/db/MockFileEntry.hpp>

#include <brfc/oh5/MockPhysicalFile.hpp>

using ::testing::Ref;
using ::testing::Return;

namespace brfc {

class db_Database_test : public ::testing::Test {
  public:
    db_Database_test()
        : db() {
    }

    MockDatabase db;
};

TEST_F(db_Database_test, test_create_invalid_dsn) {
    EXPECT_THROW(Database::create("invalid_dsn"), value_error);
}

TEST_F(db_Database_test, test_get_or_store_stored) {
    oh5::MockPhysicalFile f; 
    MockFileEntry fe;
    EXPECT_CALL(db, do_is_stored(Ref(f)))
        .WillOnce(Return(true));
    EXPECT_CALL(db, do_entry_by_file(Ref(f)))
        .WillOnce(Return(&fe));
    
    EXPECT_EQ(&fe, db.get_or_store(f));
}

TEST_F(db_Database_test, test_get_or_store_not_stored) {
    oh5::MockPhysicalFile f; 
    MockFileEntry fe;
    EXPECT_CALL(db, do_is_stored(Ref(f)))
        .WillOnce(Return(false));
    EXPECT_CALL(db, do_store(Ref(f)))
        .WillOnce(Return(&fe));
    
    EXPECT_EQ(&fe, db.get_or_store(f));
}

} // namespace brfc
