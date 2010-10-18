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

#include <boost/filesystem.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/FileCatalog.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test/TempH5File.hpp>
#include <brfc/test/TempDir.hpp>

#include "common.hpp"
#include "MockLocalStorage.hpp"
#include "db/MockDatabase.hpp"
#include "db/MockFileEntry.hpp"

using testing::_;
using testing::Eq;
using testing::DefaultValue;
using testing::Property;
using testing::Ref;
using testing::Return;
using testing::Throw;

namespace fs = boost::filesystem;

namespace brfc {

struct FileCatalog_test : public testing::Test {
    FileCatalog_test()
            : tempdir(new test::TempDir())
            , db()
            , entry(new db::MockFileEntry())
            , storage()
            , fc(&db, &storage)
            , tempfile()
            , minfile("PVOL", Date(2000, 1, 1), Time(12, 0), "WMO:02606") {
    }

    virtual void SetUp() {
        tempfile.write(minfile);
        minfile.path(tempfile.path());
    }

    auto_ptr<test::TempDir> tempdir;
    db::MockDatabase db;
    shared_ptr<db::MockFileEntry> entry;
    MockLocalStorage storage;
    FileCatalog fc;
    test::TempH5File tempfile;
    oh5::hl::HlFile minfile;
};

TEST_F(FileCatalog_test, test_invalid_dsn_throws) {
    EXPECT_THROW(FileCatalog("invalid_dsn"), value_error);
}

TEST_F(FileCatalog_test, test_store_nx_file_by_path) {
    EXPECT_THROW(fc.store("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_store) {
    EXPECT_CALL(db, do_store(Ref(minfile)))
        .WillOnce(Return(entry));
    EXPECT_CALL(storage, do_prestore(Ref(*entry), minfile.path()))
        .WillOnce(Return("/path/to/file"));
    
    shared_ptr<const db::FileEntry> e;
    EXPECT_NO_THROW(e = fc.store(minfile));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_store_on_db_failure) {
    String orig_path = minfile.path();

    EXPECT_CALL(db, do_store(Ref(minfile)))
        .WillOnce(Throw(db_error("")));

     // propagates db_error
    EXPECT_THROW(fc.store(minfile), db_error);
    // retains original location
    EXPECT_EQ(orig_path, minfile.path());
}

TEST_F(FileCatalog_test, test_store_on_prestore_failure) {
    EXPECT_CALL(db, do_store(Ref(minfile)))
        .WillOnce(Return(entry));
    EXPECT_CALL(storage, do_prestore(Ref(*entry), minfile.path()))
        .WillOnce(Throw(std::runtime_error("error")));
    
    shared_ptr<const db::FileEntry> e;
    EXPECT_NO_THROW(e = fc.store(minfile));
    EXPECT_TRUE(e);
}

/*
TEST_F(FileCatalog_test, test_double_import_throws) {
    EXPECT_THROW(fc.store(minfile), duplicate_entry);
}
*/

TEST_F(FileCatalog_test, test_is_stored_nx_file_by_path) {
    EXPECT_THROW(fc.is_stored("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_is_stored_on_new_file) {
    EXPECT_CALL(db, do_is_stored(Ref(minfile)))
        .WillOnce(Return(false));

    EXPECT_FALSE(fc.is_stored(minfile));
}

TEST_F(FileCatalog_test, test_remove_existing_file) {
    EXPECT_CALL(db, do_remove(Ref(*entry)));
    EXPECT_CALL(storage, do_remove(Ref(*entry)))
        .WillOnce(Return(true));

    EXPECT_NO_THROW(fc.remove(*entry));
}

TEST_F(FileCatalog_test, test_remove_nx_file) {
    EXPECT_CALL(db, do_remove(Ref(*entry)))
        .WillOnce(Return(true));
    EXPECT_CALL(storage, do_remove(Ref(*entry)))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(fc.remove(*entry));
}

} // namespace brfc
