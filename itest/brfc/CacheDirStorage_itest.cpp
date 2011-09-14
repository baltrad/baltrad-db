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

#include <boost/filesystem.hpp>

#include <brfc/CacheDirStorage.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

#include <brfc/test/TempDir.hpp>

#include <brfc/util/BoostFileSystem.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

#include <brfc/test_common.hpp>
#include <brfc/db/MockFileEntry.hpp>

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;

namespace fs = boost::filesystem;

namespace brfc {

class CacheDirStorage_itest : public ::testing::Test {
  public:
    CacheDirStorage_itest()
            : dir()
            , storage(dir.path())
            , tmpfile()
            , file("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo")
            , entry() {
    }

    virtual void SetUp() {
        Oh5HlFileWriter writer;
        writer.write(file, tmpfile.path());
        file.path(tmpfile.path());
        ON_CALL(entry, do_uuid()).WillByDefault(Return("uuid"));
    }

    void copy_tmpfile(const std::string& path) {
        fs.copy_file(tmpfile.path(), path);
    }
    
    test::TempDir dir;
    CacheDirStorage storage;
    NamedTemporaryFile tmpfile;
    BoostFileSystem fs;
    Oh5File file;
    ::testing::NiceMock<MockFileEntry> entry;
};

TEST_F(CacheDirStorage_itest, test_prestore) {
    const std::string& fpath = file.path();
    const std::string& rpath = storage.prestore(entry, fpath);

    EXPECT_NE(rpath, fpath);
    EXPECT_TRUE(fs::exists(rpath));
    EXPECT_TRUE(fs::exists(fpath));
}

TEST_F(CacheDirStorage_itest, test_store) {
    EXPECT_CALL(entry, do_write_to_file(_))
        .WillOnce(Invoke(this, &CacheDirStorage_itest::copy_tmpfile));

    const std::string& rpath = storage.store(entry);

    EXPECT_TRUE(fs::exists(rpath));
}

TEST_F(CacheDirStorage_itest, test_double_store) {
    EXPECT_CALL(entry, do_write_to_file(_))
        .WillOnce(Invoke(this, &CacheDirStorage_itest::copy_tmpfile));
    
    const std::string& rpath1 = storage.store(entry);
    EXPECT_TRUE(fs::exists(rpath1));
    
    const std::string& rpath2 = storage.store(entry);
    EXPECT_TRUE(fs::exists(rpath2));
    
    EXPECT_EQ(rpath1, rpath2);
}

TEST_F(CacheDirStorage_itest, test_remove) {
    const std::string& path = storage.entry_path(entry);
    fs.copy_file(tmpfile.path(), path);

    EXPECT_TRUE(storage.remove(entry));
    EXPECT_FALSE(fs::exists(path));
}

TEST_F(CacheDirStorage_itest, test_remove_nx) {
    const std::string& path = storage.entry_path(entry);

    EXPECT_TRUE(storage.remove(entry));
    EXPECT_FALSE(fs::exists(path));
}

TEST_F(CacheDirStorage_itest, test_clean) {
    const std::string& path = storage.entry_path(entry);
    fs.copy_file(tmpfile.path(), path);

    EXPECT_NO_THROW(storage.clean());
    EXPECT_FALSE(fs::exists(path));
}

} // namespace brfc
