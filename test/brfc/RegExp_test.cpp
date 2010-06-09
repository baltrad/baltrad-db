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

#include <stdexcept>

#include <gtest/gtest.h>

#include <brfc/RegExp.hpp>

namespace brfc {

TEST(RegExp_test, test_index_in) {
    RegExp re("[0-9]+");

    EXPECT_EQ(-1, re.index_in("qweqweasdasd"));

    String str("123 blabla 456asd789");
    EXPECT_EQ(0, re.index_in(str));
    EXPECT_EQ(1, re.index_in(str, 1));
    EXPECT_EQ(11, re.index_in(str, 4));
}

TEST(RegExp_test, test_matched_length) {
    RegExp re("[0-9]+");

    re.index_in("qweqweasdasd");
    EXPECT_EQ(-1, re.matched_length());

    String str("123 blabla 456asd789");
    re.index_in(str);
    EXPECT_EQ(3, re.matched_length());
    re.index_in(str, 1);
    EXPECT_EQ(2, re.matched_length());
    re.index_in(str, 4);
    EXPECT_EQ(3, re.matched_length());
}

TEST(RegExp_test, test_cap) {
    RegExp re("[0-9]+");

    re.index_in("qweqweqwe");
    EXPECT_EQ("", re.cap());

    String str("123 blabla 456asd789");
    re.index_in(str);
    EXPECT_EQ("123", re.cap());
    re.index_in(str, 1);
    EXPECT_EQ("23", re.cap());
    re.index_in(str, 4);
    EXPECT_EQ("456", re.cap());
}

TEST(RegExp_test, test_pos) { 
    RegExp re("[0-9]+");

    re.index_in("qweqweasdasd");
    EXPECT_EQ(-1, re.pos());

    String str("123 blabla 456asd789");
    re.index_in(str);
    EXPECT_EQ(0, re.pos());
    re.index_in(str, 1);
    EXPECT_EQ(1, re.pos());
    re.index_in(str, 4);
    EXPECT_EQ(11, re.pos());
}

TEST(RegExp_test, test_group_count) {
    RegExp re1("([0-9]+)(abc)(cde)");
    EXPECT_EQ(re1.group_count(), 3);

    RegExp re2("");
    EXPECT_EQ(re2.group_count(), 0);
}

TEST(RegExp_test, test_group) {
    RegExp re("([0-9]+)[a-z]+([0-9]+)");
    EXPECT_TRUE(re.match("0a1"));
    EXPECT_EQ("0", re.group(1));
    EXPECT_EQ("1", re.group(2));

    EXPECT_FALSE(re.match("aabbcc1"));
    EXPECT_THROW(re.group(1), std::runtime_error);

    EXPECT_FALSE(re.match("1aabb"));
    EXPECT_THROW(re.group(1), std::runtime_error);
}

} // namespace brfc
