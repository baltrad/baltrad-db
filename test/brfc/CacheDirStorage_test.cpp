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

#include <brfc/CacheDirStorage.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/db/MockFileEntry.hpp>
#include <brfc/util/MockFileSystem.hpp>

using ::testing::InSequence;
using ::testing::Return;

namespace brfc {

class CacheDirStorage_test : public ::testing::Test {
  public:
    CacheDirStorage_test()
            : dir("/tmp")
            , fs()
            , entry()
            , storage(dir, &fs) {
    }
    
    std::string dir;
    MockFileSystem fs;
    db::MockFileEntry entry;
    CacheDirStorage storage;

    virtual void SetUp() {
        EXPECT_CALL(entry, do_uuid())
            .WillRepeatedly(Return("uuid"));
    }

};

TEST_F(CacheDirStorage_test, test_store) {
    EXPECT_CALL(fs, do_join(dir, "uuid.h5"))
        .WillOnce(Return(dir + "/uuid.h5"));
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillOnce(Return(false));
    EXPECT_CALL(entry, do_write_to_file("/tmp/uuid.h5"));

    EXPECT_EQ("/tmp/uuid.h5", storage.store(entry));
}

TEST_F(CacheDirStorage_test, test_store_existing) {
    EXPECT_CALL(fs, do_join(dir, "uuid.h5"))
        .WillOnce(Return(dir + "/uuid.h5"));
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillOnce(Return(true));

    EXPECT_EQ("/tmp/uuid.h5", storage.store(entry));
}

TEST_F(CacheDirStorage_test, test_prestore) {
    EXPECT_CALL(fs, do_join(dir, "uuid.h5"))
        .WillOnce(Return(dir + "/uuid.h5"));
    EXPECT_CALL(fs, do_copy_file("/infile.h5", "/tmp/uuid.h5"));

    storage.prestore(entry, "/infile.h5");
}

TEST_F(CacheDirStorage_test, test_remove) {
    {
        InSequence s;
        EXPECT_CALL(fs, do_join(dir, "uuid.h5"))
            .WillOnce(Return(dir + "/uuid.h5"));
        EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
            .WillOnce(Return(true));
        EXPECT_CALL(fs, do_remove("/tmp/uuid.h5"));
        EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
            .WillRepeatedly(Return(false));
    }

    EXPECT_TRUE(storage.remove(entry));
}

TEST_F(CacheDirStorage_test, test_remove_nx_entry) {
    EXPECT_CALL(fs, do_join(dir, "uuid.h5"))
        .WillOnce(Return(dir + "/uuid.h5"));
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillRepeatedly(Return(false));
    
    EXPECT_TRUE(storage.remove(entry));
}


TEST_F(CacheDirStorage_test, test_clean) {
    EXPECT_CALL(fs, do_clear_directory(dir));

    storage.clean();
}

TEST_F(CacheDirStorage_test, test_is_valid) {
    EXPECT_CALL(fs, do_is_absolute(dir))
        .WillOnce(Return(true));
    EXPECT_CALL(fs, do_exists(dir))
        .WillOnce(Return(true));
    EXPECT_CALL(fs, do_is_directory(dir))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(storage.is_valid());
}

TEST_F(CacheDirStorage_test, test_is_valid_not_absolute) {
    EXPECT_CALL(fs, do_is_absolute(dir))
        .WillOnce(Return(false));
    
    EXPECT_FALSE(storage.is_valid());
}

TEST_F(CacheDirStorage_test, test_is_valid_nonexistant_dir) {
    EXPECT_CALL(fs, do_is_absolute(dir))
        .WillOnce(Return(true));
    EXPECT_CALL(fs, do_exists(dir))
        .WillOnce(Return(false));
    
    EXPECT_FALSE(storage.is_valid());
}

TEST_F(CacheDirStorage_test, test_is_valid_not_directory) {
    EXPECT_CALL(fs, do_is_absolute(dir))
        .WillOnce(Return(true));
    EXPECT_CALL(fs, do_exists(dir))
        .WillOnce(Return(true));
    EXPECT_CALL(fs, do_is_directory(dir))
        .WillOnce(Return(false));
    
    EXPECT_FALSE(storage.is_valid());
}

} // namespace brfc
