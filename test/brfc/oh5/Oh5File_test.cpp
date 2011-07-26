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
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/MemoryOh5NodeBackend.hpp>
#include <brfc/oh5/Oh5Source.hpp>

#include <brfc/test_common.hpp>
#include <brfc/oh5/MockOh5File.hpp>

using ::testing::ReturnRef;

namespace brfc {

struct oh5_Oh5File_test : public testing::Test {
    oh5_Oh5File_test()
            : file()
            , node_backend()
            , root(node_backend.root())
            , what() {
    }

    virtual void SetUp() {
        ON_CALL(file, do_root())
            .WillByDefault(ReturnRef(root));

        what = static_cast<Oh5Group*>(&root.add(new Oh5Group("what")));
        what_object = &what->add(new Oh5Attribute("object", Oh5Scalar("pvol")));
        what->add(new Oh5Attribute("date", Oh5Scalar(Date(2000, 1, 2))));
        what->add(new Oh5Attribute("time", Oh5Scalar(Time(12, 5))));
        what->add(new Oh5Attribute("source", Oh5Scalar("WMO:02606")));
    }

    ::testing::NiceMock<MockOh5File> file;
    MemoryOh5NodeBackend node_backend;
    Oh5Node& root;
    Oh5Group* what;
    Oh5Node* what_object;
};

TEST_F(oh5_Oh5File_test, test_group) {
    EXPECT_EQ(&root, file.group("/"));
    EXPECT_EQ(what, file.group("/what"));
    EXPECT_FALSE(file.group("/what/source"));
    EXPECT_FALSE(file.group("/nx"));
}

TEST_F(oh5_Oh5File_test, test_attribute) {
    EXPECT_FALSE(file.attribute("/"));
    EXPECT_FALSE(file.attribute("/what"));
    EXPECT_EQ(what_object, file.attribute("/what/object"));
}

TEST_F(oh5_Oh5File_test, test_node) {
    EXPECT_EQ(&root, file.node("/"));
    EXPECT_EQ(what, file.node("/what"));
    EXPECT_TRUE(file.node("/what/source"));
    EXPECT_FALSE(file.node("/nx"));
}

TEST_F(oh5_Oh5File_test, required_attribute_shortcuts) {
    EXPECT_EQ(file.what_object(), "pvol");
    EXPECT_EQ(file.what_date(), Date(2000, 1, 2));
    EXPECT_EQ(file.what_time(), Time(12, 5));
    EXPECT_EQ(file.what_source(), "WMO:02606");
}

TEST_F(oh5_Oh5File_test, required_attribute_shortcuts_when_missing) {
    MemoryOh5NodeBackend be;
    EXPECT_CALL(file, do_root())
        .WillRepeatedly(ReturnRef(be.root()));

    EXPECT_THROW(file.what_object(), lookup_error);
    EXPECT_THROW(file.what_date(), lookup_error);
    EXPECT_THROW(file.what_time(), lookup_error);
    EXPECT_THROW(file.what_source(), lookup_error);
}

TEST_F(oh5_Oh5File_test, what_date_conversion) {
    what->attribute("date")->value(Oh5Scalar("20001213"));
    EXPECT_EQ(Date(2000, 12, 13), file.what_date());

    what->attribute("date")->value(Oh5Scalar("foo"));
    EXPECT_THROW(file.what_date(), std::invalid_argument);
}

TEST_F(oh5_Oh5File_test, what_time_conversion) {
    what->attribute("time")->value(Oh5Scalar("123456"));
    EXPECT_EQ(Time(12, 34, 56), file.what_time());

    what->attribute("time")->value(Oh5Scalar("bar"));
    EXPECT_THROW(file.what_time(), std::invalid_argument);
}

TEST_F(oh5_Oh5File_test, test_source_get) {
    EXPECT_EQ("WMO:02606", file.source().to_string());
    
    // missing /what/source
    MemoryOh5NodeBackend be;
    EXPECT_CALL(file, do_root())
        .WillRepeatedly(ReturnRef(be.root()));

    EXPECT_EQ("", file.source().to_string());
}

} // namespace brfc
