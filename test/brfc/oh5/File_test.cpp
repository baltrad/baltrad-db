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
#include <gmock/gmock.h>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include "../common.hpp"
#include "MockFile.hpp"

using ::testing::ReturnRef;

namespace brfc {
namespace oh5 {

struct oh5_File_test : public testing::Test {
    oh5_File_test()
            : file()
            , root()
            , emptyroot()
            , what(root.create_group("what")) {
    }

    virtual void SetUp() {
        ON_CALL(file, do_root())
            .WillByDefault(ReturnRef(root));

        what.create_attribute("object", Scalar("pvol"));
        what.create_attribute("date", Scalar(Date(2000, 1, 2)));
        what.create_attribute("time", Scalar(Time(12, 5)));
        what.create_attribute("source", Scalar("WMO:02606"));
    }

    ::testing::NiceMock<MockFile> file;
    RootGroup root, emptyroot;
    Group& what;
};

TEST_F(oh5_File_test, test_group) {
    EXPECT_EQ(&root, file.group("/"));
    EXPECT_EQ(&what, file.group("/what"));
    EXPECT_FALSE(file.group("/what/source"));
    EXPECT_FALSE(file.group("/nx"));
}

TEST_F(oh5_File_test, required_attribute_shortcuts) {
    EXPECT_EQ(file.what_object(), "pvol");
    EXPECT_EQ(file.what_date(), Date(2000, 1, 2));
    EXPECT_EQ(file.what_time(), Time(12, 5));
    EXPECT_EQ(file.what_source(), "WMO:02606");
}

TEST_F(oh5_File_test, required_attribute_shortcuts_when_missing) {
    EXPECT_CALL(file, do_root())
        .WillRepeatedly(ReturnRef(emptyroot));

    EXPECT_THROW(file.what_object(), lookup_error);
    EXPECT_THROW(file.what_date(), lookup_error);
    EXPECT_THROW(file.what_time(), lookup_error);
    EXPECT_THROW(file.what_source(), lookup_error);
}

TEST_F(oh5_File_test, what_date_conversion) {
    what.attribute("date")->value(Scalar("20001213"));
    EXPECT_EQ(Date(2000, 12, 13), file.what_date());

    what.attribute("date")->value(Scalar("foo"));
    EXPECT_THROW(file.what_date(), value_error);
}

TEST_F(oh5_File_test, what_time_conversion) {
    what.attribute("time")->value(Scalar("123456"));
    EXPECT_EQ(Time(12, 34, 56), file.what_time());

    what.attribute("time")->value(Scalar("bar"));
    EXPECT_THROW(file.what_time(), value_error);
}

TEST_F(oh5_File_test, test_name) {
    String path;
    EXPECT_CALL(file, do_path())
        .WillRepeatedly(ReturnRef(path));
    
    EXPECT_EQ(file.name(), "");

    path = "/path/to/filename";
    EXPECT_EQ(file.name(), "filename");

    path = "filename2";
    EXPECT_EQ(file.name(), "filename2");
}

TEST_F(oh5_File_test, test_source_get) {
    EXPECT_EQ("WMO:02606", file.source().to_string());

    // missing /what/source
    EXPECT_CALL(file, do_root())
        .WillRepeatedly(ReturnRef(emptyroot));

    EXPECT_EQ("", file.source().to_string());
}

TEST_F(oh5_File_test, test_source_set) {
    Source s;
    s.add("WMO", "02606");
    s.add("RAD", "SE50");
    const String& expected = s.to_string();

    file.source(s);
    EXPECT_EQ(expected, file.what_source());

    file.source(s);
    ASSERT_NO_THROW(file.what_source());
    EXPECT_EQ(expected, file.what_source());
}

} // namespace oh5
} // namespace brfc
