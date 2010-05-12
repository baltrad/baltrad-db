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
#include <brfc/String.hpp>
#include <brfc/StringList.hpp>

namespace brfc {

class String_test : public ::testing::Test {
  public:
    String_test()
            : empty()
            , ascii("abcdef")
            , unicode("€ öäü κόσμε")
            , empty_nchars(0)
            , ascii_nchars(6)
            , unicode_nchars(11)
            , empty_noctets(0)
            , ascii_noctets(6)
            , unicode_noctets(21) {

    }

    String empty, ascii, unicode;
    int empty_nchars, ascii_nchars, unicode_nchars;
    int empty_noctets, ascii_noctets, unicode_noctets;
};

TEST_F(String_test, test_eq) {
    EXPECT_TRUE(empty == empty);
    EXPECT_FALSE(empty != empty);
    // explicit empty
    EXPECT_TRUE(empty == String(""));

    EXPECT_TRUE(ascii == ascii);
    EXPECT_FALSE(ascii != ascii);

    EXPECT_TRUE(empty != ascii);
    EXPECT_FALSE(empty == ascii);

    EXPECT_TRUE(ascii != unicode);
    EXPECT_FALSE(ascii == unicode);
}

TEST_F(String_test, test_length) {
    EXPECT_EQ(empty_nchars, empty.length());
    EXPECT_EQ(ascii_nchars, ascii.length());
    EXPECT_EQ(unicode_nchars, unicode.length());
}

TEST_F(String_test, test_to_utf8) {
    std::string utf8 = empty.to_utf8();
    EXPECT_EQ(empty_noctets, utf8.length());

    utf8 = ascii.to_utf8();
    EXPECT_EQ(ascii_noctets, utf8.length());

    utf8 = unicode.to_utf8();
    EXPECT_EQ(unicode_noctets, utf8.length());
}

TEST_F(String_test, test_from_to_utf8) {
    EXPECT_EQ(empty, String(empty.to_utf8()));
    EXPECT_EQ(ascii, String(ascii.to_utf8()));
    EXPECT_EQ(unicode, String(unicode.to_utf8()));
}

TEST_F(String_test, test_from_to_utf16) {
    EXPECT_EQ(empty, String::from_utf16(empty.utf16()));
    EXPECT_EQ(ascii, String::from_utf16(ascii.utf16()));
    EXPECT_EQ(unicode, String::from_utf16(unicode.utf16()));

    EXPECT_EQ(empty, String::from_utf16(empty.utf16(), empty.length()));
    EXPECT_EQ(ascii, String::from_utf16(ascii.utf16(), ascii.length()));
    EXPECT_EQ(unicode, String::from_utf16(unicode.utf16(), unicode.length()));
}

TEST_F(String_test, test_number_long_long) {
    long long pos = 9223372036854775807LL;
    long long neg = -9223372036854775807LL;

    EXPECT_EQ("9223372036854775807", String::number(pos));
    EXPECT_EQ("-9223372036854775807", String::number(neg));
}

TEST_F(String_test, test_number_double) {
    double pos = 1.234567;
    double neg = -1.234567;

    EXPECT_EQ("1.234567", String::number(pos));
    EXPECT_EQ("-1.234567", String::number(neg));
}

TEST_F(String_test, test_split_normal) {
    StringList elms = String("a,bb,ccc").split(",");
    ASSERT_EQ(3, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("ccc", elms.at(2));
}

TEST_F(String_test, test_split_with_keep_empty) {
    StringList elms = String("a,bb,,ccc,").split(",", String::KEEP_EMPTY_PARTS);
    ASSERT_EQ(5, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("", elms.at(2));
    EXPECT_EQ("ccc", elms.at(3));
    EXPECT_EQ("", elms.at(4));
}

TEST_F(String_test, test_split_with_skip_empty) {
    StringList elms = String("a,bb,,ccc,").split(",", String::SKIP_EMPTY_PARTS);
    ASSERT_EQ(3, elms.size());
    EXPECT_EQ("a", elms.at(0));
    EXPECT_EQ("bb", elms.at(1));
    EXPECT_EQ("ccc", elms.at(2));
}

TEST_F(String_test, test_split_with_sep_only_and_keep_empty) {
    StringList elms = String("/").split("/", String::KEEP_EMPTY_PARTS);
    ASSERT_EQ(2, elms.size());
    EXPECT_EQ("", elms.at(0));
    EXPECT_EQ("", elms.at(1));
}

TEST_F(String_test, test_split_with_sep_only_and_skip_empty) {
    StringList elms = String("/").split("/", String::SKIP_EMPTY_PARTS);
    EXPECT_EQ(0, elms.size());
}

TEST_F(String_test, tes_split_with_empty_sep) {
    EXPECT_THROW(String("asd").split(""), value_error);
}

TEST_F(String_test, test_stringlist_join) {
    StringList list;
    EXPECT_EQ("", list.join("/"));
    list.append("qwe");
    EXPECT_EQ("qwe", list.join("/"));
    list.append("asd");
    EXPECT_EQ("qwe/asd", list.join("/"));
}

TEST_F(String_test, test_stringlist_contains) {
    StringList list;
    EXPECT_FALSE(list.contains("qwe"));
    list.append("qwe");
    EXPECT_TRUE(list.contains("qwe"));
}

TEST_F(String_test, test_append) {
    EXPECT_EQ("asdqwe", String("asd").append("qwe"));
    EXPECT_EQ("asd", String("asd").append(""));
    EXPECT_EQ("asdq", String("asd").append('q'));
}

TEST_F(String_test, test_prepend) {
    EXPECT_EQ("qweasd", String("asd").prepend("qwe"));
    EXPECT_EQ("asd", String("asd").prepend(""));
}

TEST_F(String_test, test_insert) {
    EXPECT_EQ("qweasdqwe", String("qweqwe").insert(3, "asd"));
    EXPECT_EQ("qweasd", String("qwe").insert(3, "asd"));
    EXPECT_THROW(String("qwe").insert(-1, "asd"), value_error);
    EXPECT_THROW(String("qwe").insert(-4, "asd"), value_error);
    EXPECT_THROW(String("qweqwe").insert(7, "asd"), value_error);
}

TEST_F(String_test, test_remove_by_substring) {
    EXPECT_EQ("whe rabb",
              String("white rabbit").remove("it"));
    EXPECT_EQ("white rabbit",
              String("white rabbit").remove(""));
}

TEST_F(String_test, test_remove_by_position) {
    EXPECT_EQ("whiterabbit",
              String("white rabbit").remove(5, 1));
    EXPECT_EQ("",
              String("white rabbit").remove(0, 100));
    EXPECT_EQ("white",
              String("white rabbit").remove(5, 100));
    EXPECT_THROW(String("white rabbit").remove(100, 1000), value_error);
    EXPECT_THROW(String("white rabbit").remove(-100, 106), value_error);
}

TEST_F(String_test, test_replace_by_substring) {
    EXPECT_EQ("whole rabbol",
              String("white rabbit").replace("it", "ol"));
    EXPECT_EQ("whisee rabbise",
              String("white rabbit").replace("t", "se"));
    EXPECT_EQ("whoe rabbo",
              String("white rabbit").replace("it", "o"));
    EXPECT_EQ("whe rabb",
              String("white rabbit").replace("it", ""));
    EXPECT_EQ("white rabbit",
              String("white rabbit").replace("", "qwe"));
}

TEST_F(String_test, test_replace_by_position) {
    EXPECT_EQ("whole rabbit",
              String("white rabbit").replace(2, 2, "ol"));
    EXPECT_EQ("whitney rabbit",
              String("white rabbit").replace(4, 1, "ney"));
    EXPECT_EQ("white rabbiqwe",
              String("white rabbit").replace(11, 2, "qwe"));
}

TEST_F(String_test, test_last_index_of) {
    String s("white rabbit");
    EXPECT_EQ(10, s.last_index_of("it"));
    EXPECT_EQ(2, s.last_index_of("it", 9));
    EXPECT_EQ(2, s.last_index_of("it", 2));
    EXPECT_EQ(-1, s.last_index_of("it", 1));
}

TEST_F(String_test, test_contains) {
    String s("white rabbit");
    EXPECT_TRUE(s.contains("it"));
    EXPECT_TRUE(s.contains(" "));
    EXPECT_TRUE(s.contains(""));
    EXPECT_TRUE(String().contains(""));
    EXPECT_FALSE(s.contains("its"));
    EXPECT_TRUE(String("asd").contains("d"));
}

TEST_F(String_test, test_starts_with) {
    String s("white rabbit");
    EXPECT_TRUE(s.starts_with("wh"));
    EXPECT_FALSE(s.starts_with("hi"));
    EXPECT_TRUE(s.starts_with(""));
    EXPECT_TRUE(String().starts_with(""));
}

TEST_F(String_test, test_right_justified) {
    EXPECT_EQ("00001", String("1").right_justified(5, '0'));
    EXPECT_EQ("11111", String("11111").right_justified(5, '0'));
    EXPECT_EQ("111111", String("111111").right_justified(5, '0'));
}

TEST_F(String_test, test_section) {
     String csv("field1,field2,field3,field4");
     String path("/path/to/file");

     EXPECT_EQ("field3", csv.section(",", 2, 2));
     EXPECT_EQ("to/file", path.section("/", 2, 3));
     EXPECT_EQ("field2,field3", csv.section(",", -3, -2));
     EXPECT_EQ("file", path.section("/", -1));
}

TEST_F(String_test, test_to_int) {
    EXPECT_EQ(2147483647, String("2147483647").to_int());
    EXPECT_EQ(-2147483647, String("-2147483647").to_int());
    EXPECT_THROW(String("2147483648").to_int(), value_error);
    EXPECT_THROW(String("-2147483649").to_int(), value_error);
    EXPECT_THROW(String("asdasd").to_int(), value_error);
}

TEST_F(String_test, test_char_at) {
    String qwe("qwe");
    EXPECT_EQ('q', qwe.char_at(0));
    EXPECT_EQ('w', qwe.char_at(1));
    EXPECT_THROW(qwe.char_at(3), value_error);
    EXPECT_THROW(qwe.char_at(-1), value_error);
}

TEST_F(String_test, test_substr) {
    String str("white rabbit");
    EXPECT_EQ("te rabbit", str.substr(3));
    EXPECT_EQ("te rabbit", str.substr(3, -1));
    EXPECT_EQ("te ra", str.substr(3, 5));
    EXPECT_EQ("te rabbit", str.substr(3, 100));
    EXPECT_EQ("t", str.substr(11));
    EXPECT_EQ("", str.substr(0, 0));
    EXPECT_THROW(str.substr(-1), value_error);
    EXPECT_THROW(str.substr(100), value_error);
}

TEST_F(String_test, test_to_lower) {
    EXPECT_EQ("qweasdasd", String("QwEasdASD").to_lower());
}

TEST_F(String_test, test_operator_plus) {
    EXPECT_EQ("qweasd", String("qwe") + "asd");
    EXPECT_EQ("qweasd", "qwe" + String("asd"));
}

TEST_F(String_test, test_operator_append) {
    EXPECT_EQ("asdqwe", String("asd") += "qwe");
    EXPECT_EQ("asd", String("asd") += "");
    EXPECT_EQ("asdq", String("asd") += 'q');
}

} // namespace brfc
