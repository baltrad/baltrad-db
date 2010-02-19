#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/Database.hpp>
#include <brfc/File.hpp>
#include <brfc/FileCatalog.hpp>
#include <brfc/Source.hpp>
#include <brfc/Query.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QVariant>
#include <QtCore/QFile>

#include "common.hpp"
#include "TempDir.hpp"
#include "TempH5File.hpp"


using testing::_;
using testing::DefaultValue;
using testing::Return;
using testing::Throw;

namespace brfc {

class MockDatabase : public Database {
  public:
    MOCK_METHOD0(do_begin, void());
    MOCK_METHOD0(do_rollback, void());
    MOCK_METHOD0(do_commit, void());
    
    MOCK_CONST_METHOD1(do_has_file, bool(const File&));
    MOCK_METHOD1(do_remove_file, void(const QString&));
    MOCK_METHOD2(do_save_file, long long(const QString&, const File&));
    MOCK_METHOD1(do_load_source, shared_ptr<Source>(const QString&));
    MOCK_METHOD1(do_query, shared_ptr<ResultSet>(const Query&));
    MOCK_METHOD0(do_clean, void());  
};

class MockNamer : public FileNamer {
  public:
    MOCK_CONST_METHOD1(do_name, QString(const File&));
};

struct FileCatalog_test : public testing::Test {
    FileCatalog_test()
            : tempdir(new TempDir())
            , db(new MockDatabase())
            , namer(new MockNamer())
            , specs(new AttributeSpecs())
            , fc(db, specs, namer)
            , src_str("WMO:02606")
            , default_src(new Source())
            , minfile(TempH5File::minimal("PVOL",
                                          QDate(2000, 1, 1),
                                          QTime(12, 0),
                                          src_str)) {
        minfile->write();
        specs->add("Conventions", "string");
        specs->add("what/object", "string");
        specs->add("what/version", "string");
        specs->add("what/date", "date");
        specs->add("what/time", "time");
        specs->add("what/source", "string");
        DefaultValue<shared_ptr<Source> >::Set(default_src);
    }

    auto_ptr<TempDir> tempdir;
    shared_ptr<MockDatabase> db;
    shared_ptr<MockNamer> namer;
    shared_ptr<AttributeSpecs> specs;
    FileCatalog fc;
    QString src_str;
    shared_ptr<Source> default_src;
    auto_ptr<TempH5File> minfile;
};

TEST_F(FileCatalog_test, test_invalid_dsn_throws) {
    EXPECT_THROW(FileCatalog("invalid_dsn", tempdir->path()), db_error);
}

TEST_F(FileCatalog_test, test_catalog) {
    const QString& target = tempdir->path() + "/test";
    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return(target));
    EXPECT_CALL(*db, do_save_file(target, _))
        .WillOnce(Return(1));
    EXPECT_CALL(*db, do_commit());
    
    // file is given the path returned by namer
    shared_ptr<File> f = fc.catalog(minfile->filename());
    EXPECT_EQ(f->path(), target);
    EXPECT_EQ(f->db_id(), 1);

    EXPECT_TRUE(QFile::exists(target));
}

TEST_F(FileCatalog_test, test_catalog_on_db_failure) {
    const QString& target = tempdir->path() + "/test";
    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return(target));
    EXPECT_CALL(*db, do_save_file(target, _))
        .WillOnce(Throw(db_error("")));
    EXPECT_CALL(*db, do_rollback());
    
    EXPECT_THROW(fc.catalog(minfile->filename()), db_error);
    EXPECT_FALSE(QFile::exists(target));
}

TEST_F(FileCatalog_test, test_catalog_on_copy_failure) {
    const QString& target = tempdir->path() + "/test";
    EXPECT_CALL(*db, do_load_source(src_str));
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));
    EXPECT_CALL(*namer, do_name(_))
        .WillOnce(Return(target));
    EXPECT_CALL(*db, do_save_file(target, _))
        .WillOnce(Return(1));
    EXPECT_CALL(*db, do_rollback());

    tempdir.reset(); // tempdir removed

    EXPECT_THROW(fc.catalog(minfile->filename()), fs_error);
    EXPECT_FALSE(QFile::exists(target));
}

TEST_F(FileCatalog_test, test_double_import_throws) {
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(true));

    EXPECT_THROW(fc.catalog(minfile->filename()), duplicate_entry);
}

TEST_F(FileCatalog_test, test_is_cataloged_on_nx_file) {
    EXPECT_THROW(fc.is_cataloged("/path/to/nxfile"), fs_error);
}

TEST_F(FileCatalog_test, test_is_cataloged_on_new_file) {
    EXPECT_CALL(*db, do_has_file(_))
        .WillOnce(Return(false));

    EXPECT_FALSE(fc.is_cataloged(minfile->filename()));
}

TEST_F(FileCatalog_test, test_remove_existing_file) {
    const QString& target = tempdir->path() + "/testfile";
    QFile f(target);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.close();

    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_remove_file(target));
    EXPECT_CALL(*db, do_commit());

    EXPECT_NO_THROW(fc.remove(target));

    EXPECT_FALSE(QFile::exists(target));
}

TEST_F(FileCatalog_test, test_removing_nx_file) {
    EXPECT_CALL(*db, do_begin());
    EXPECT_CALL(*db, do_remove_file(QString("/path/to/nxfile")));
    EXPECT_CALL(*db, do_rollback());

    EXPECT_THROW(fc.remove("/path/to/nxfile"), fs_error);
}

} // namespace brfc
