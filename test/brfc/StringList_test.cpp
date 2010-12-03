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

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

namespace brfc {

TEST(StringList_test, test_split_normal) {
    StringList elms = StringList::split("a,bb,ccc", ",");
    ASSERT_EQ(3, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("ccc", elms.at(2));
}

TEST(StringList_test, test_split_with_keep_empty) {
    StringList elms = StringList::split("a,bb,,ccc,", ",", StringList::KEEP_EMPTY_PARTS);
    ASSERT_EQ(5, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("", elms.at(2));
    EXPECT_EQ("ccc", elms.at(3));
    EXPECT_EQ("", elms.at(4));
}

TEST(StringList_test, test_split_with_skip_empty) {
    StringList elms = StringList::split("a,bb,,ccc,", ",", StringList::SKIP_EMPTY_PARTS);
    ASSERT_EQ(3, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("ccc", elms.at(2));
}

TEST(StringList_test, test_split_with_sep_only_and_keep_empty) {
    StringList elms = StringList::split("/", "/", StringList::KEEP_EMPTY_PARTS);
    ASSERT_EQ(2, elms.size());
    EXPECT_EQ("", elms.at(0));
    EXPECT_EQ("", elms.at(1));
}

TEST(StringList_test, test_split_with_sep_only_and_skip_empty) {
    StringList elms = StringList::split("/", "/", StringList::SKIP_EMPTY_PARTS);
    EXPECT_EQ(0, elms.size());
}

TEST(StringList_test, test_split_with_empty_sep) {
    EXPECT_THROW(StringList::split("asd", ""), value_error);
}

TEST(StringList_test, test_join) {
    StringList list;
    EXPECT_EQ("", list.join("/"));
    list.append("qwe");
    EXPECT_EQ("qwe", list.join("/"));
    list.append("asd");
    EXPECT_EQ("qwe/asd", list.join("/"));
}

TEST(StringList_test, test_contains) {
    StringList list;
    EXPECT_FALSE(list.contains("qwe"));
    list.append("qwe");
    EXPECT_TRUE(list.contains("qwe"));
}

} // namespace brfc
