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
#include <brfc/DateTimeParser.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

namespace {

class FakeIntToken : public dttok::IntToken {
  public:
    FakeIntToken(int max_length, bool zero_prefixed, bool signed_=false)
            : dttok::IntToken(max_length, zero_prefixed, signed_) {
    }

    int value() const { return value_; }

    void value(int value) { value_ = value; }
  
  protected:
    virtual void set_value(DateTime&, int value) const {
        value_ = value;
    }

    virtual int get_value(const DateTime&) const {
        return value_;
    }
 
  private:
    mutable int value_;
};

} // namespace anonymous

TEST(DateTimeParser_test, test_int_token_non_zero_prefixed_unsigned) {
    DateTime dt;
    FakeIntToken t(2, false);

    EXPECT_EQ(t.max_length(), 2);

    EXPECT_EQ(1, t.to_datetime(dt, "9"));
    EXPECT_EQ(9, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "10"));
    EXPECT_EQ(10, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "123"));
    EXPECT_EQ(12, t.value());

    EXPECT_THROW(t.to_datetime(dt, "-123"), value_error);
    EXPECT_THROW(t.to_datetime(dt, "-1"), value_error);
}

TEST(DateTimeParser_test, test_int_token_zero_prefixed_unsigned) {
    DateTime dt;
    FakeIntToken t(2, true);

    EXPECT_EQ(t.max_length(), 2);

    EXPECT_EQ(2, t.to_datetime(dt, "08"));
    EXPECT_EQ(8, t.value());
    
    EXPECT_EQ(2, t.to_datetime(dt, "15"));
    EXPECT_EQ(15, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "123"));
    EXPECT_EQ(12, t.value());
    
    EXPECT_THROW(t.to_datetime(dt, "1"), value_error);
    EXPECT_THROW(t.to_datetime(dt, ""), value_error);
    EXPECT_THROW(t.to_datetime(dt, "-1"), value_error);
    EXPECT_THROW(t.to_datetime(dt, "-123"), value_error);
}

TEST(DateTimeParser_test, test_int_token_non_zero_prefixed_signed) {
    DateTime dt;
    FakeIntToken t(2, false, true);

    EXPECT_EQ(t.max_length(), 3);

    EXPECT_EQ(1, t.to_datetime(dt, "9"));
    EXPECT_EQ(9, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "10"));
    EXPECT_EQ(10, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "321"));
    EXPECT_EQ(32, t.value());

    EXPECT_EQ(3, t.to_datetime(dt, "-12"));
    EXPECT_EQ(-12, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "-1"));
    EXPECT_EQ(-1, t.value());
}

TEST(DateTimeParser_test, test_int_token_zero_prefixed_signed) {
    DateTime dt;
    FakeIntToken t(2, true, true);

    EXPECT_EQ(t.max_length(), 3);

    EXPECT_EQ(2, t.to_datetime(dt, "10"));
    EXPECT_EQ(10, t.value());

    EXPECT_EQ(3, t.to_datetime(dt, "-12"));
    EXPECT_EQ(-12, t.value());

    EXPECT_EQ(3, t.to_datetime(dt, "-01"));
    EXPECT_EQ(-1, t.value());

    EXPECT_EQ(3, t.to_datetime(dt, "-321"));
    EXPECT_EQ(-32, t.value());

    EXPECT_EQ(2, t.to_datetime(dt, "456"));
    EXPECT_EQ(45, t.value());

    EXPECT_THROW(t.to_datetime(dt, "-1"), value_error);
    EXPECT_THROW(t.to_datetime(dt, "-"), value_error);
    EXPECT_THROW(t.to_datetime(dt, "9"), value_error);
}

TEST(DateTimeParser_test, test_literal_token) {
    DateTime dt;
    dttok::Literal l("qwe");
    
    EXPECT_EQ(l.max_length(), 3);

    int consumed = 0;
    EXPECT_NO_THROW(consumed = l.to_datetime(dt, "qwe"));
    EXPECT_EQ(3, consumed);

    EXPECT_NO_THROW(consumed = l.to_datetime(dt, "qweasd"));
    EXPECT_EQ(3, consumed);

    EXPECT_THROW(l.to_datetime(dt, "q"), value_error);
    EXPECT_THROW(l.to_datetime(dt, "qasd"), value_error);
}

namespace {

template<typename T>
shared_ptr<T>
get_token_at(DateTimeParser::TokenVector& format, size_t idx) {
    return dynamic_pointer_cast<T>(format.at(idx));
}

} // namespace anonymous

TEST(DateTimeParser_test, test_tokenize_format1) {
    DateTimeParser::TokenVector f = DateTimeParser::tokenize("yyyy/MM/dd'yy'hhmmqwesszzz");
    shared_ptr<dttok::Literal> lit;

    EXPECT_EQ(11, f.size());
    ASSERT_GT(f.size(), 0);
    EXPECT_TRUE(get_token_at<dttok::Year>(f, 0));

    ASSERT_GT(f.size(), 1);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 1));
    if (lit) { EXPECT_EQ("/", lit->value()); }

    ASSERT_GT(f.size(), 2);
    EXPECT_TRUE(get_token_at<dttok::Month>(f, 2));

    ASSERT_GT(f.size(), 3);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 3));
    if (lit) { EXPECT_EQ("/", lit->value()); }

    ASSERT_GT(f.size(), 4);
    EXPECT_TRUE(get_token_at<dttok::Day>(f, 4));

    ASSERT_GT(f.size(), 5);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 5));
    if (lit) { EXPECT_EQ("yy", lit->value()); }

    ASSERT_GT(f.size(), 6);
    EXPECT_TRUE(get_token_at<dttok::Hour>(f, 6));

    ASSERT_GT(f.size(), 7);
    EXPECT_TRUE(get_token_at<dttok::Minute>(f, 7));

    ASSERT_GT(f.size(), 8);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 8));
    if (lit) { EXPECT_EQ("qwe", lit->value()); }

    ASSERT_GT(f.size(), 9);
    EXPECT_TRUE(get_token_at<dttok::Second>(f, 9));

    ASSERT_GT(f.size(), 10);
    EXPECT_TRUE(get_token_at<dttok::Msec>(f, 10));
}

TEST(DateTimeParser_test, test_tokenize_format2) {
    DateTimeParser::TokenVector f = DateTimeParser::tokenize("aedyyyM''zz");
    shared_ptr<dttok::Literal> lit;

    EXPECT_EQ(6, f.size());
    ASSERT_GT(f.size(), 0);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 0));
    if (lit) { EXPECT_EQ("ae", lit->value()); }

    ASSERT_GT(f.size(), 1);
    EXPECT_TRUE(get_token_at<dttok::Day>(f, 1));

    ASSERT_GT(f.size(), 2);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 2));
    if (lit) { EXPECT_EQ("yyy", lit->value()); }

    ASSERT_GT(f.size(), 3);
    EXPECT_TRUE(get_token_at<dttok::Month>(f, 3));

    ASSERT_GT(f.size(), 4);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 4));
    if (lit) { EXPECT_EQ("'", lit->value()); }

    ASSERT_GT(f.size(), 5);
    EXPECT_TRUE(lit = get_token_at<dttok::Literal>(f, 5));
    if (lit) { EXPECT_EQ("zz", lit->value()); }
}

TEST(DateTimeParser_test, test_is_format_date_only) {
    EXPECT_TRUE(DateTimeParser("yyyyMMdd").is_format_date_only());
    EXPECT_FALSE(DateTimeParser("yyyyMMddh").is_format_date_only());
}

TEST(DateTimeParser_test, test_is_format_time_only) {
    EXPECT_TRUE(DateTimeParser("hhmmsszzz").is_format_time_only());
    EXPECT_FALSE(DateTimeParser("hhmmsszzzd").is_format_time_only());
}

TEST(DateTimeParser_test, test_from_string) {
    DateTimeParser parser("yyyy/MM/dd hh:mm:ss.zzz");
    DateTime dt;
    EXPECT_NO_THROW(dt = parser.from_string("2005/12/13 12:34:56.789"));
    EXPECT_EQ(Date(2005, 12, 13), dt.date());
    EXPECT_EQ(Time(12, 34, 56, 789), dt.time());
}

TEST(DateTimeParser_test, test_from_string_invalid_literal) {
    DateTimeParser parser("yyyy/MM/dd");
    DateTime dt;
    EXPECT_THROW(dt = parser.from_string("2000-01-02"), value_error);
    EXPECT_NE(Date(2000, 1, 2), dt.date());
}

TEST(DateTimeParser_test, test_to_string) {
    DateTimeParser parser("yyyy/MM/dd hh:mm:ss.zzz");
    DateTime dt(2005, 12, 13, 12, 34, 56, 789);
    std::string str;
    EXPECT_NO_THROW(str = parser.to_string(dt));
    EXPECT_EQ("2005/12/13 12:34:56.789", str);
}

} // namespace brfc
