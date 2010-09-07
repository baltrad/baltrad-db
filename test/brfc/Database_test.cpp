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

#include <brfc/oh5/File.hpp>

#include "MockDatabase.hpp"
#include "MockFileEntry.hpp"

using ::testing::Ref;
using ::testing::Return;

namespace brfc {

class Database_test : public ::testing::Test {
  public:
    Database_test()
        : db() {
    }

    MockDatabase db;
};

TEST_F(Database_test, test_save_associates_file_with_entry) {
    shared_ptr<FileEntry> e(new MockFileEntry());

    shared_ptr<oh5::File> f(oh5::File::create());

    EXPECT_CALL(db, do_save_file(Ref(*f)))
        .WillOnce(Return(e));
    
    e = db.save_file(*f);
    
    EXPECT_TRUE(e->file());
    EXPECT_EQ(f, e->file());
}

} // namespace brfc
