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
#include <brfc/Database.hpp>
#include <brfc/Date.hpp>
#include <brfc/FileCatalog.hpp>
#include <brfc/FileEntry.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/hl/File.hpp>

#include <brfc/test/TempH5File.hpp>
#include <brfc/test/TempDir.hpp>

#include "common.hpp"
#include "MockDatabase.hpp"
#include "MockFileEntry.hpp"
#include "MockLocalStorage.hpp"

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
            , db(new MockDatabase())
            , entry(new MockFileEntry())
            , storage(new MockLocalStorage())
            , fc(db, storage)
            , tempfile()
            , minfile("PVOL", Date(2000, 1, 1), Time(12, 0), "WMO:02606") {
    }

    virtual void SetUp() {
        tempfile.write(minfile);
        minfile.path(tempfile.path());
    }

    auto_ptr<test::TempDir> tempdir;
    shared_ptr<MockDatabase> db;
    shared_ptr<MockFileEntry> entry;
    shared_ptr<MockLocalStorage> storage;
    FileCatalog fc;
    test::TempH5File tempfile;
    oh5::hl::File minfile;
};

TEST_F(FileCatalog_test, test_invalid_dsn_throws) {
    EXPECT_THROW(FileCatalog("invalid_dsn"), value_error);
}

TEST_F(FileCatalog_test, test_catalog_nx_file_by_path) {
    EXPECT_THROW(fc.catalog("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_catalog) {
    EXPECT_CALL(*db, do_save_file(Ref(minfile)))
        .WillOnce(Return(entry));
    EXPECT_CALL(*storage, do_prestore(Ref(*entry), minfile.path()))
        .WillOnce(Return("/path/to/file"));
    
    shared_ptr<const FileEntry> e;
    EXPECT_NO_THROW(e = fc.catalog(minfile));
    EXPECT_TRUE(e);
}

TEST_F(FileCatalog_test, test_catalog_on_db_failure) {
    String orig_path = minfile.path();

    EXPECT_CALL(*db, do_save_file(Ref(minfile)))
        .WillOnce(Throw(db_error("")));

     // propagates db_error
    EXPECT_THROW(fc.catalog(minfile), db_error);
    // retains original location
    EXPECT_EQ(orig_path, minfile.path());
}

TEST_F(FileCatalog_test, test_catalog_on_prestore_failure) {
    EXPECT_CALL(*db, do_save_file(Ref(minfile)))
        .WillOnce(Return(entry));
    EXPECT_CALL(*storage, do_prestore(Ref(*entry), minfile.path()))
        .WillOnce(Throw(std::runtime_error("error")));
    
    shared_ptr<const FileEntry> e;
    EXPECT_NO_THROW(e = fc.catalog(minfile));
    EXPECT_TRUE(e);
}

/*
TEST_F(FileCatalog_test, test_double_import_throws) {
    EXPECT_THROW(fc.catalog(minfile), duplicate_entry);
}
*/

TEST_F(FileCatalog_test, test_is_cataloged_nx_file_by_path) {
    EXPECT_THROW(fc.is_cataloged("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_is_cataloged_on_new_file) {
    EXPECT_CALL(*db, do_has_file(Ref(minfile)))
        .WillOnce(Return(false));

    EXPECT_FALSE(fc.is_cataloged(minfile));
}

TEST_F(FileCatalog_test, test_remove_existing_file) {
    EXPECT_CALL(*db, do_remove_file(Ref(*entry)));
    EXPECT_CALL(*storage, do_remove(Ref(*entry)))
        .WillOnce(Return(true));

    EXPECT_NO_THROW(fc.remove(*entry));
}

TEST_F(FileCatalog_test, test_remove_nx_file) {
    EXPECT_CALL(*db, do_remove_file(Ref(*entry)))
        .WillOnce(Return(true));
    EXPECT_CALL(*storage, do_remove(Ref(*entry)))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(fc.remove(*entry));
}

} // namespace brfc
