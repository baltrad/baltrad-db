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
along with baltrad-db. If not, see <scheme://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/util/Url.hpp>

namespace brfc {

TEST(util_Url_test, test_ctor1) {
    Url url("scheme://user:password@example.com:1234/path/to/resource");
    EXPECT_EQ("scheme", url.scheme());
    EXPECT_EQ("user", url.user_name());
    EXPECT_EQ("password", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(1234, url.port());
    EXPECT_EQ("path/to/resource", url.path());
}

TEST(util_Url_test, test_ctor2) {
    Url url("scheme://user@example.com:1234/path/to/resource");
    EXPECT_EQ("scheme", url.scheme());
    EXPECT_EQ("user", url.user_name());
    EXPECT_EQ("", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(1234, url.port());
    EXPECT_EQ("path/to/resource", url.path());
}

TEST(util_Url_test, test_ctor3) {
    Url url("scheme://user@example.com/path/to/resource");
    EXPECT_EQ("scheme", url.scheme());
    EXPECT_EQ("user", url.user_name());
    EXPECT_EQ("", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(0, url.port());
    EXPECT_EQ("path/to/resource", url.path());
}

TEST(util_Url_test, test_ctor4) {
    Url url("scheme://example.com/path/to/resource");
    EXPECT_EQ("scheme", url.scheme());
    EXPECT_EQ("", url.user_name());
    EXPECT_EQ("", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(0, url.port());
    EXPECT_EQ("path/to/resource", url.path());
}

TEST(util_Url_test, test_ctor5) {
    Url url("scheme://example.com");
    EXPECT_EQ("scheme", url.scheme());
    EXPECT_EQ("", url.user_name());
    EXPECT_EQ("", url.password());
    EXPECT_EQ("example.com", url.host());
    EXPECT_EQ(0, url.port());
    EXPECT_EQ("", url.path());
}

TEST(util_Url_test, test_invalid_url_throws) {
    EXPECT_THROW(Url("invalid_url"), std::invalid_argument);
    EXPECT_THROW(Url("scheme://example.com:port/"), std::invalid_argument);
    EXPECT_THROW(Url("scheme:///"), std::invalid_argument);
    EXPECT_THROW(Url("scheme://"), std::invalid_argument);
    EXPECT_THROW(Url("scheme://:@:/"), std::invalid_argument);
}

TEST(util_Url_test, test_http_path) {
    Url url;
    url.path("path/to/resource?arg1=val1&arg2=val2");
    EXPECT_EQ("path/to/resource", url.http_path());
}

TEST(util_Url_test, test_http_searchpart) {
    Url url;
    url.path("path/to/resource?arg1=val1&arg2=val2&arg3=val3");
    std::map<std::string, std::string> q = url.http_searchpart();
    EXPECT_EQ(3u, q.size());
    EXPECT_TRUE(q.find("arg1") != q.end());
    EXPECT_TRUE(q.find("arg2") != q.end());
    EXPECT_TRUE(q.find("arg3") != q.end());
    EXPECT_EQ("val1", q["arg1"]);
    EXPECT_EQ("val2", q["arg2"]);
    EXPECT_EQ("val3", q["arg3"]);
}

TEST(util_Url_test, test_http_searchpath_empty) {
    Url url;
    url.path("path/to/resource");
    std::map<std::string, std::string> q;
    EXPECT_NO_THROW(q = url.http_searchpart());
    EXPECT_EQ(0u, q.size());

    url.path("path/to/resource?");
    EXPECT_NO_THROW(q = url.http_searchpart());
    EXPECT_EQ(0u, q.size());
}

} // namespace brfc
