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

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

namespace brfc {

class CacheDirStorage_test : public ::testing::Test {
  public:
    CacheDirStorage_test()
            : dir("/tmp")
            , fs()
            , entry()
            , storage(dir, 3) {
    }
    
    std::string dir;
    MockFileSystem fs;
    MockFileEntry entry;
    CacheDirStorage storage;

    virtual void SetUp() {
        storage.file_system(&fs);
        EXPECT_CALL(entry, do_uuid())
            .WillRepeatedly(Return("uuid"));

        ON_CALL(fs, do_join(_, _))
            .WillByDefault(Invoke(this, &CacheDirStorage_test::join_path));
    }

    std::string join_path(const std::string& a, const std::string& b) {
        return a + "/" + b;
    }

};

TEST_F(CacheDirStorage_test, test_init) {
    std::vector<std::string> entries;
    entries.push_back("a");
    entries.push_back("b");
    entries.push_back("c");
    entries.push_back("d");

    EXPECT_CALL(fs, do_list_directory(dir))
        .WillOnce(Return(entries));
    
    EXPECT_CALL(fs, do_remove("/tmp/a")); // exceeds overflow
    
    storage.init();
}

TEST_F(CacheDirStorage_test, test_store) {
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillOnce(Return(false));
    EXPECT_CALL(entry, do_write_to_file("/tmp/uuid.h5"));

    EXPECT_EQ("/tmp/uuid.h5", storage.store(entry));
}

TEST_F(CacheDirStorage_test, test_store_existing) {
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillOnce(Return(true));

    EXPECT_EQ("/tmp/uuid.h5", storage.store(entry));
}

TEST_F(CacheDirStorage_test, test_store_overflow) {
    std::vector<std::string> entries;
    entries.push_back("a");
    entries.push_back("b");
    entries.push_back("c");

    EXPECT_CALL(fs, do_list_directory(dir))
        .WillOnce(Return(entries));

    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillOnce(Return(false));
    EXPECT_CALL(entry, do_write_to_file("/tmp/uuid.h5"));
    EXPECT_CALL(fs, do_remove("/tmp/a"));
    
    storage.init();
    storage.store(entry);
}

TEST_F(CacheDirStorage_test, test_prestore) {
    EXPECT_CALL(fs, do_copy_file("/infile.h5", "/tmp/uuid.h5"));

    storage.store(entry, "/infile.h5");
}

TEST_F(CacheDirStorage_test, test_prestore_overflow) {
    std::vector<std::string> entries;
    entries.push_back("a");
    entries.push_back("b");
    entries.push_back("c");

    EXPECT_CALL(fs, do_list_directory(dir))
        .WillOnce(Return(entries));
    EXPECT_CALL(fs, do_copy_file("/infile.h5", "/tmp/uuid.h5"));
    EXPECT_CALL(fs, do_remove("/tmp/a"));
    
    storage.init();
    storage.store(entry, "/infile.h5");
}

TEST_F(CacheDirStorage_test, test_remove) {
    std::vector<std::string> files;
    files.push_back("uuid.h5");

    EXPECT_CALL(fs, do_list_directory("/tmp"))
        .WillOnce(Return(files));
    EXPECT_CALL(fs, do_remove("/tmp/uuid.h5"));
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillRepeatedly(Return(false));
    
    storage.init();
    EXPECT_TRUE(storage.remove(entry));
}

TEST_F(CacheDirStorage_test, test_remove_nx_entry) {
    EXPECT_CALL(fs, do_exists("/tmp/uuid.h5"))
        .WillRepeatedly(Return(false));
    
    EXPECT_TRUE(storage.remove(entry));
}

} // namespace brfc
