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
#include <brfc/oh5/Source.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

TEST(oh5_Source_test, test_from_string_valid) {
    Source s = Source::from_string("WMO:02606,RAD:SE50");

    EXPECT_EQ((size_t)2, s.keys().size());

    EXPECT_TRUE(s.has("WMO"));
    EXPECT_TRUE(s.has("RAD"));

    EXPECT_EQ("02606", s.get("WMO"));
    EXPECT_EQ("SE50", s.get("RAD"));
}

TEST(oh5_Source_test, test_from_string_empty) {
    Source s = Source::from_string("");
    EXPECT_EQ((size_t)0, s.keys().size());
}

TEST(oh5_Source_test, test_from_string_skip_empty) {
    Source s = Source::from_string("WMO:");
    EXPECT_FALSE(s.has("WMO"));
}

TEST(oh5_Source_test, test_from_string_invalid) {
    EXPECT_THROW(Source::from_string("a:b,sd"), value_error);
    EXPECT_THROW(Source::from_string("a:b:c"), value_error);
    EXPECT_THROW(Source::from_string("a,b,"), value_error);
}

TEST(oh5_Source_test, test_add) {
    Source s;
    EXPECT_FALSE(s.has("qwe"));

    s.add("qwe", "asd");
    EXPECT_TRUE(s.has("qwe"));

    EXPECT_THROW(s.add("qwe", "123"), duplicate_entry);
}

TEST(oh5_Source_test, test_at) {
    Source s;
    EXPECT_THROW(s.get("qwe"), lookup_error);

    s.add("qwe", "asd");
    EXPECT_EQ("asd", s.get("qwe"));
}

TEST(oh5_Source_test, test_keys) {
    Source s;
    EXPECT_EQ((size_t)0, s.keys().size());
    s.add("qwe", "asd");
    EXPECT_EQ((size_t)1, s.keys().size());
    EXPECT_TRUE(s.keys().contains("qwe"));
}

TEST(oh5_Source_test, test_remove) {
    Source s;
    EXPECT_THROW(s.remove("qwe"), lookup_error);
    s.add("qwe", "asd");
    EXPECT_TRUE(s.has("qwe"));
    EXPECT_NO_THROW(s.remove("qwe"));
    EXPECT_FALSE(s.has("qwe"));
}

TEST(oh5_Source_test, test_to_string) {
    Source s;
    EXPECT_EQ("", s.to_string());
    s.add("WMO", "02606");
    EXPECT_EQ("WMO:02606", s.to_string());
    s.add("RAD", "SE50");
    EXPECT_EQ("RAD:SE50,WMO:02606", s.to_string());
    s.add("PLC", "Ängelholm");
    EXPECT_EQ("PLC:Ängelholm,RAD:SE50,WMO:02606", s.to_string());
}

} // namespace oh5
} // namespace brfc
