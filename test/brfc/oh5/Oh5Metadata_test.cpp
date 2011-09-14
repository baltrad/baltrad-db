/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

This meta is part of baltrad-db.

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
#include <brfc/oh5/Oh5Metadata.hpp>
#include <brfc/oh5/Oh5Source.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class oh5_Oh5Metadata_test : public ::testing::Test {
  public:
    oh5_Oh5Metadata_test()
            : meta()
            , root(0)
            , what(0)
            , what_object(0) {

    }

    virtual void SetUp() {
        root = &meta.root();
        what = &root->add(new Oh5Group("what"));
        what_object = &what->add(new Oh5Attribute("object", Oh5Scalar("pvol")));
        what->add(new Oh5Attribute("date", Oh5Scalar(Date(2000, 1, 2))));
        what->add(new Oh5Attribute("time", Oh5Scalar(Time(12, 5))));
        what->add(new Oh5Attribute("source", Oh5Scalar("WMO:02606")));
    }
    
    Oh5Metadata meta;
    Oh5Node* root, *what, *what_object;
};

TEST_F(oh5_Oh5Metadata_test, test_group) {
    EXPECT_EQ(root, meta.group("/"));
    EXPECT_EQ(what, meta.group("/what"));
    EXPECT_FALSE(meta.group("/what/source"));
    EXPECT_FALSE(meta.group("/nx"));
}

TEST_F(oh5_Oh5Metadata_test, test_attribute) {
    EXPECT_FALSE(meta.attribute("/"));
    EXPECT_FALSE(meta.attribute("/what"));
    EXPECT_EQ(what_object, meta.attribute("/what/object"));
}

TEST_F(oh5_Oh5Metadata_test, test_node) {
    EXPECT_EQ(root, meta.node("/"));
    EXPECT_EQ(what, meta.node("/what"));
    EXPECT_TRUE(meta.node("/what/source"));
    EXPECT_FALSE(meta.node("/nx"));
}

TEST_F(oh5_Oh5Metadata_test, required_attribute_shortcuts) {
    EXPECT_EQ(meta.what_object(), "pvol");
    EXPECT_EQ(meta.what_date(), Date(2000, 1, 2));
    EXPECT_EQ(meta.what_time(), Time(12, 5));
    EXPECT_EQ(meta.what_source(), "WMO:02606");
}

TEST_F(oh5_Oh5Metadata_test, required_attribute_shortcuts_when_missing) {
    Oh5Metadata empty;

    EXPECT_THROW(empty.what_object(), lookup_error);
    EXPECT_THROW(empty.what_date(), lookup_error);
    EXPECT_THROW(empty.what_time(), lookup_error);
    EXPECT_THROW(empty.what_source(), lookup_error);
}

TEST_F(oh5_Oh5Metadata_test, what_date_conversion) {
    what->attribute("date")->value(Oh5Scalar("20001213"));
    EXPECT_EQ(Date(2000, 12, 13), meta.what_date());

    what->attribute("date")->value(Oh5Scalar("foo"));
    EXPECT_THROW(meta.what_date(), std::invalid_argument);
}

TEST_F(oh5_Oh5Metadata_test, what_time_conversion) {
    what->attribute("time")->value(Oh5Scalar("123456"));
    EXPECT_EQ(Time(12, 34, 56), meta.what_time());

    what->attribute("time")->value(Oh5Scalar("bar"));
    EXPECT_THROW(meta.what_time(), std::invalid_argument);
}

TEST_F(oh5_Oh5Metadata_test, test_get_source) {
    EXPECT_EQ("WMO:02606", meta.source().to_string());    
}

TEST_F(oh5_Oh5Metadata_test, test_get_source_missing) {
    Oh5Metadata empty;
    EXPECT_EQ("", empty.source().to_string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_object) {
    meta.what_object("scan");
    Oh5Attribute* a = meta.attribute("/what/object");
    ASSERT_TRUE(a);
    EXPECT_EQ("scan", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_object_new) {
    Oh5Metadata m;
    m.what_object("scan");
    Oh5Attribute* a = m.attribute("/what/object");
    ASSERT_TRUE(a);
    EXPECT_EQ("scan", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_date) {
    meta.what_date(Date(2011, 1, 2));
    Oh5Attribute* a = meta.attribute("/what/date");
    ASSERT_TRUE(a);
    EXPECT_EQ("20110102", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_date_new) {
    Oh5Metadata m;
    m.what_date(Date(2011, 1, 2));
    Oh5Attribute* a = m.attribute("/what/date");
    ASSERT_TRUE(a);
    EXPECT_EQ("20110102", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_time) {
    meta.what_time(Time(11, 12, 13));
    Oh5Attribute* a = meta.attribute("/what/time");
    ASSERT_TRUE(a);
    EXPECT_EQ("111213", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_time_new) {
    Oh5Metadata m;
    m.what_time(Time(11, 12, 13));
    Oh5Attribute* a = m.attribute("/what/time");
    ASSERT_TRUE(a);
    EXPECT_EQ("111213", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_source) {
    meta.what_source("WMO:02666");
    Oh5Attribute* a = meta.attribute("/what/source");
    ASSERT_TRUE(a);
    EXPECT_EQ("WMO:02666", a->value().string());
}

TEST_F(oh5_Oh5Metadata_test, test_set_what_source_new) {
    Oh5Metadata m;
    m.what_source("WMO:02666");
    Oh5Attribute* a = m.attribute("/what/source");
    ASSERT_TRUE(a);
    EXPECT_EQ("WMO:02666", a->value().string());
}


} // namespace brfc
