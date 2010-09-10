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

#include <brfc/CacheDirStorage.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/File.hpp>

#include <brfc/test/TempDir.hpp>
#include <brfc/test/TempH5File.hpp>

#include "common.hpp"
#include "MockFileEntry.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;

namespace fs = boost::filesystem;

namespace brfc {

class CacheDirStorage_test : public ::testing::Test {
  public:
    CacheDirStorage_test()
            : dir()
            , storage(dir.path())
            , tmpfile()
            , file(oh5::File::minimal("PVOL", Date(2000, 1, 1), Time(12, 0), "PLC:Legionowo"))
            , entry() {
    }

    virtual void SetUp() {
        tmpfile.write(*file);
        file->path(tmpfile.path());
        ON_CALL(entry, do_id()).WillByDefault(Return(1));
    }
    
    test::TempDir dir;
    CacheDirStorage storage;
    test::TempH5File tmpfile;
    shared_ptr<oh5::File> file;
    ::testing::NiceMock<MockFileEntry> entry;
};

TEST_F(CacheDirStorage_test, test_prestore) {
    entry.file(file); // associate with local file

    shared_ptr<const oh5::File> rfile = storage.prestore(entry);

    EXPECT_NE(rfile->path().to_utf8(), file->path().to_utf8());
    EXPECT_TRUE(fs::exists(rfile->path().to_utf8()));
    EXPECT_TRUE(fs::exists(file->path().to_utf8()));
}

TEST_F(CacheDirStorage_test, test_store) {
    EXPECT_CALL(entry, do_write_to_file(_))
        .WillOnce(Invoke(&tmpfile, &test::TempH5File::copy));

    shared_ptr<const oh5::File> rf = storage.store(entry);
}

TEST_F(CacheDirStorage_test, test_double_store) {
    EXPECT_CALL(entry, do_write_to_file(_))
        .WillOnce(Invoke(&tmpfile, &test::TempH5File::copy));
    
    shared_ptr<const oh5::File> rfile1 = storage.store(entry);
    EXPECT_TRUE(fs::exists(rfile1->path().to_utf8()));
    
    shared_ptr<const oh5::File> rfile2 = storage.store(entry);
    EXPECT_TRUE(fs::exists(rfile2->path().to_utf8()));
    
    EXPECT_EQ(rfile1->path(), rfile2->path());
}

TEST_F(CacheDirStorage_test, test_remove) {
    const String& path = storage.entry_path(entry);
    tmpfile.copy(path);

    EXPECT_TRUE(storage.remove(entry));
    EXPECT_FALSE(fs::exists(path.to_utf8()));
}

TEST_F(CacheDirStorage_test, test_remove_nx) {
    const String& path = storage.entry_path(entry);

    EXPECT_TRUE(storage.remove(entry));
    EXPECT_FALSE(fs::exists(path.to_utf8()));
}

TEST_F(CacheDirStorage_test, test_clean) {
    const String& path = storage.entry_path(entry);
    tmpfile.copy(path);

    EXPECT_NO_THROW(storage.clean());
    EXPECT_FALSE(fs::exists(path.to_utf8()));
}

} // namespace brfc
