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

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/MemoryNodeBackend.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/test_common.hpp>
#include <brfc/db/MockFileEntry.hpp>
#include <brfc/oh5/MockFile.hpp>

using ::testing::Return;
using ::testing::ReturnRef;

namespace brfc {

class DefaultFileNamer_test : public ::testing::Test {
  public:
    DefaultFileNamer_test()
            : namer()
            , root()
            , what()
            , file()
            , entry() {
    }
    
    virtual void SetUp() {
        root.backend(new oh5::MemoryNodeBackend());
        ON_CALL(entry, do_uuid())
            .WillByDefault(Return("abcd0123-0000-0000-0000-000000000000"));
        ON_CALL(entry, do_root())
            .WillByDefault(ReturnRef(root));
        ON_CALL(file, do_root())
            .WillByDefault(ReturnRef(root));

        what = &root.create_group("what");
        what->create_attribute("object", oh5::Scalar("pvol"));
        what->create_attribute("date", oh5::Scalar(Date(2010, 11, 12)));
        what->create_attribute("time", oh5::Scalar(Time(14, 15)));
    }
    
    DefaultFileNamer namer;
    oh5::RootGroup root;
    oh5::Group* what;
    ::testing::NiceMock<oh5::MockFile> file;
    ::testing::NiceMock<db::MockFileEntry> entry;
};

TEST_F(DefaultFileNamer_test, name_file) {
    what->create_attribute("source", oh5::Scalar("_name:seang"));
    
    EXPECT_EQ("pvol_seang_20101112T141500Z.h5", namer.name(file));
}

TEST_F(DefaultFileNamer_test, name_file_unknown_source) {
    EXPECT_EQ("pvol_unknown_20101112T141500Z.h5", namer.name(file));
}

TEST_F(DefaultFileNamer_test, name_entry) {
    what->create_attribute("source", oh5::Scalar("_name:seang"));

    ON_CALL(entry, do_source())
        .WillByDefault(Return(oh5::Source::from_string("_name:seang")));

    EXPECT_EQ(namer.name(entry), "pvol_seang_20101112T141500Z_abcd0123.h5");
}

} // namespace brfc
