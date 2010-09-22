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

#include <brfc/util/SHA1.hpp>

class SHA1_test : public ::testing::Test {
  public:
    SHA1_test()
        : sha1()
        , hash() {
    }
    
    CSHA1 sha1;
    std::string hash;
};

// test messages from FIPS PUB 180-1
TEST_F(SHA1_test, test_fips180_1_sample_1) {
    std::string str("abc");

    sha1.Update((unsigned char*)str.c_str(), str.length());
    sha1.Final();
    sha1.ReportHashStl(hash, CSHA1::REPORT_HEX_SHORT);
    EXPECT_EQ("A9993E364706816ABA3E25717850C26C9CD0D89D", hash);
}

TEST_F(SHA1_test, test_fips180_1_sample_2) {
    std::string str("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    sha1.Update((unsigned char*)str.c_str(), str.length());
    sha1.Final();
    sha1.ReportHashStl(hash, CSHA1::REPORT_HEX_SHORT);
    EXPECT_EQ("84983E441C3BD26EBAAE4AA1F95129E5E54670F1", hash);
}

TEST_F(SHA1_test, test_fips180_1_sample3) {
    std::string str;
    std::string a50 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    str.reserve(1000000);
    for (int i = 0; i < 20000; ++i) {
        str += a50;
    }
    
    sha1.Update((unsigned char*)str.c_str(), str.length());
    sha1.Final();
    sha1.ReportHashStl(hash, CSHA1::REPORT_HEX_SHORT);
    EXPECT_EQ("34AA973CD4C4DAA4F61EEB2BDBAD27316534016F", hash);
}
