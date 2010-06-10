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
#include <brfc/FileNamer.hpp>
#include <brfc/Query.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/test/TempH5File.hpp>
#include <brfc/test/TempDir.hpp>

#include "common.hpp"
#include "MockDatabase.hpp"


using testing::_;
using testing::Eq;
using testing::DefaultValue;
using testing::Property;
using testing::Return;
using testing::Throw;

namespace fs = boost::filesystem;

namespace brfc {

class MockNamer : public FileNamer {
  public:
    MOCK_CONST_METHOD1(do_name, String(const oh5::File&));
};

struct FileCatalog_test : public testing::Test {
    FileCatalog_test()
            : tempdir(new test::TempDir())
            , db(new MockDatabase())
            , namer(new MockNamer())
            , specs(new oh5::AttributeSpecs())
            , fc(db, specs, namer, tempdir->path())
            , src_str("WMO:02606")
            , default_src(make_shared<oh5::SourceRadar>())
            , tempfile()
            , minfile(oh5::File::minimal("PVOL",
                                         Date(2000, 1, 1),
                                         Time(12, 0),
                                         src_str)) {
    }

    virtual void SetUp() {
        tempfile.write(*minfile);
        minfile->path(tempfile.path());
        specs->add(oh5::AttributeSpec("Conventions", "string"));
        specs->add(oh5::AttributeSpec("what/object", "string"));
        specs->add(oh5::AttributeSpec("what/version", "string"));
        specs->add(oh5::AttributeSpec("what/date", "date"));
        specs->add(oh5::AttributeSpec("what/time", "time"));
        specs->add(oh5::AttributeSpec("what/source", "string"));
        DefaultValue<shared_ptr<oh5::Source> >::Set(default_src);
    }

    auto_ptr<test::TempDir> tempdir;
    shared_ptr<MockDatabase> db;
    shared_ptr<MockNamer> namer;
    shared_ptr<oh5::AttributeSpecs> specs;
    FileCatalog fc;
    String src_str;
    shared_ptr<oh5::Source> default_src;
    test::TempH5File tempfile;
    shared_ptr<oh5::File> minfile;
};

TEST_F(FileCatalog_test, test_invalid_dsn_throws) {
    EXPECT_THROW(FileCatalog("invalid_dsn", tempdir->path()), value_error);
}

TEST_F(FileCatalog_test, test_catalog_nx_file_by_path) {
    EXPECT_THROW(fc.catalog("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_catalog) {
    const String& target = tempdir->path() + "/test_000010";
    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return("test"));
    EXPECT_CALL(*db, do_next_filename_version(Eq("test")))
        .WillOnce(Return(10));
    EXPECT_CALL(*db, do_save_file(Property(&oh5::File::path, Eq(target)),
                                  Eq("test"),
                                  Eq((unsigned int)10)))
        .WillOnce(Return(1));
    EXPECT_CALL(*db, do_commit());
    
    fc.catalog(*minfile);
    // file is given the correct path
    EXPECT_EQ(target, minfile->path());
    // file exists
    EXPECT_TRUE(fs::exists(target.to_utf8()));
}

TEST_F(FileCatalog_test, test_catalog_on_db_failure) {
    const String& target = tempdir->path() + "/test_000010";
    String orig_path = minfile->path();

    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return("test"));
    EXPECT_CALL(*db, do_next_filename_version(Eq("test")))
        .WillOnce(Return(10));
    EXPECT_CALL(*db, do_save_file(_, _, _))
        .WillOnce(Throw(db_error("")));
    EXPECT_CALL(*db, do_rollback());

     // propagates db_error
    EXPECT_THROW(fc.catalog(*minfile), db_error);
    // file not copied
    EXPECT_FALSE(fs::exists(target.to_utf8()));
    // retains original location
    EXPECT_EQ(orig_path, minfile->path());
}

TEST_F(FileCatalog_test, test_catalog_on_copy_failure) {
    const String& target = tempdir->path() + "/test_000010";
    String orig_path = minfile->path();

    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return("test"));
    EXPECT_CALL(*db, do_next_filename_version(Eq("test")))
        .WillOnce(Return(10));
    EXPECT_CALL(*db, do_save_file(_, _, _))
        .WillOnce(Return(1));
    EXPECT_CALL(*db, do_rollback());

    tempdir.reset(); // tempdir removed
    
    // propagates fs_error
    EXPECT_THROW(fc.catalog(*minfile), fs_error);
    // file not copied
    EXPECT_FALSE(fs::exists(target.to_utf8()));
    // retains original location
    EXPECT_EQ(orig_path, minfile->path());
}

TEST_F(FileCatalog_test, test_double_import_throws) {
    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(true));

    EXPECT_THROW(fc.catalog(*minfile), duplicate_entry);
}

TEST_F(FileCatalog_test, test_is_cataloged_nx_file_by_path) {
    EXPECT_THROW(fc.is_cataloged("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_is_cataloged_on_new_file) {
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));

    EXPECT_FALSE(fc.is_cataloged(*minfile));
}

TEST_F(FileCatalog_test, test_remove_existing_file) {
    const String& target = tempfile.path();

    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_remove_file(target));
    EXPECT_CALL(*db, do_commit());

    EXPECT_NO_THROW(fc.remove(target));

    EXPECT_FALSE(fs::exists(target.to_utf8()));
}

TEST_F(FileCatalog_test, test_remove_nx_file) {
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_remove_file(String("/path/to/nxfile")));
    EXPECT_CALL(*db, do_rollback());

    EXPECT_THROW(fc.remove("/path/to/nxfile"), fs_error);
}

} // namespace brfc
