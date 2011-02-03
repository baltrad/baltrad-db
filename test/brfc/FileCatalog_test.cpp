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

#include <brfc/exceptions.hpp>
#include <brfc/FileCatalog.hpp>

#include <brfc/test_common.hpp>
#include <brfc/MockLocalStorage.hpp>
#include <brfc/db/MockDatabase.hpp>
#include <brfc/db/MockFileEntry.hpp>
#include <brfc/oh5/MockPhysicalFile.hpp>

using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Throw;

namespace brfc {

struct FileCatalog_test : public ::testing::Test {
    FileCatalog_test()
            : db()
            , entry()
            , storage()
            , fc(&db, &storage)
            , path("/tmp/mockphsycialfile")
            , file() {
    }

    virtual void SetUp() {
        ON_CALL(file, do_path())
            .WillByDefault(ReturnRef(path));
        
        fc.storage(&storage);
    }

    db::MockDatabase db;
    db::MockFileEntry entry;
    MockLocalStorage storage;
    FileCatalog fc;
    std::string path;
    ::testing::NiceMock<oh5::MockPhysicalFile> file;
};

TEST_F(FileCatalog_test, test_ctor_null_db) {
    EXPECT_THROW(FileCatalog(0, &storage), value_error);
}

TEST_F(FileCatalog_test, test_ctor_null_storage) {
    EXPECT_THROW(FileCatalog(&db, 0), value_error);
}

TEST_F(FileCatalog_test, test_database_nullptr) {
    EXPECT_THROW(fc.database(0), value_error);
    EXPECT_EQ(&db, &fc.database());
}

TEST_F(FileCatalog_test, test_storage_nullptr) {
    EXPECT_THROW(fc.storage(0), value_error);
    EXPECT_EQ(&storage, &fc.storage());
}

TEST_F(FileCatalog_test, test_store_nx_file_by_path) {
    EXPECT_THROW(fc.store("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_store) {
    EXPECT_CALL(db, do_store(Ref(file)))
        .WillOnce(Return(&entry));
    EXPECT_CALL(storage, do_prestore(Ref(entry), file.path()))
        .WillOnce(Return("/path/to/file"));
    
    db::FileEntry* e = 0;
    EXPECT_NO_THROW(e = fc.store(file));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_store_on_db_failure) {
    std::string orig_path = file.path();

    EXPECT_CALL(db, do_store(Ref(file)))
        .WillOnce(Throw(db_error("")));

     // propagates db_error
    EXPECT_THROW(fc.store(file), db_error);
    // retains original location
    EXPECT_EQ(orig_path, file.path());
}

TEST_F(FileCatalog_test, test_store_on_prestore_failure) {
    EXPECT_CALL(db, do_store(Ref(file)))
        .WillOnce(Return(&entry));
    EXPECT_CALL(storage, do_prestore(Ref(entry), file.path()))
        .WillOnce(Throw(std::runtime_error("error")));
    
    db::FileEntry* e = 0;
    EXPECT_NO_THROW(e = fc.store(file));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_get_or_store) {
    EXPECT_CALL(db, do_is_stored(Ref(file)))
        .WillOnce(Return(true));
    EXPECT_CALL(db, do_entry_by_file(Ref(file)))
        .WillOnce(Return(&entry));
    EXPECT_CALL(storage, do_prestore(Ref(entry), file.path()))
        .WillOnce(Return("/path/to/file"));
    
    db::FileEntry* e = 0;
    EXPECT_NO_THROW(e = fc.get_or_store(file));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_get_or_store_on_db_failure) {
    EXPECT_CALL(db, do_is_stored(Ref(file)))
        .WillOnce(Throw(db_error("error")));

    EXPECT_THROW(fc.get_or_store(file), db_error);
}

TEST_F(FileCatalog_test, test_get_or_store_on_prestore_failure) {
    EXPECT_CALL(db, do_is_stored(Ref(file)))
        .WillOnce(Return(true));
    EXPECT_CALL(db, do_entry_by_file(Ref(file)))
        .WillOnce(Return(&entry));
    EXPECT_CALL(storage, do_prestore(Ref(entry), file.path()))
        .WillOnce(Throw(std::runtime_error("error")));
    
    db::FileEntry* e = 0;
    EXPECT_NO_THROW(e = fc.get_or_store(file));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_is_stored_nx_file_by_path) {
    EXPECT_THROW(fc.is_stored("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_is_stored_on_new_file) {
    EXPECT_CALL(db, do_is_stored(Ref(file)))
        .WillOnce(Return(false));

    EXPECT_FALSE(fc.is_stored(file));
}

TEST_F(FileCatalog_test, test_remove_existing_file) {
    EXPECT_CALL(db, do_remove(Ref(entry)));
    EXPECT_CALL(storage, do_remove(Ref(entry)))
        .WillOnce(Return(true));

    EXPECT_NO_THROW(fc.remove(entry));
}

TEST_F(FileCatalog_test, test_remove_nx_file) {
    EXPECT_CALL(db, do_remove(Ref(entry)))
        .WillOnce(Return(true));
    EXPECT_CALL(storage, do_remove(Ref(entry)))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(fc.remove(entry));
}

TEST_F(FileCatalog_test, test_local_path_for_uuid) {
    db::MockFileEntry* e = new db::MockFileEntry();
    ON_CALL(*e, do_uuid()).WillByDefault(Return("")); // for leak detection

    std::string uuid = "uuid";
    EXPECT_CALL(db, do_entry_by_uuid(Ref(uuid)))
        .WillOnce(Return(e));
    EXPECT_CALL(storage, do_store(Ref(*e)))
        .WillOnce(Return("/path/to/file"));
    
    EXPECT_EQ("/path/to/file", fc.local_path_for_uuid(uuid));
}

} // namespace brfc
