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
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/SHA1AttributeHasher.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

class SHA1AttributeHasher_test : public ::testing::Test {
  public:
    SHA1AttributeHasher_test()
            : f1("pvol", Date(2000, 1, 2), Time(12, 5), "WMO:02606")
            , f2("pvol", Date(2001, 1, 2), Time(12, 5), "WMO:02606")
            // same as f1
            , f3("pvol", Date(2000, 1, 2), Time(12, 5), "WMO:02606")
            , hasher() {
        
    }

    virtual void SetUp() {
        StringList ignored;
        ignored.push_back("Conventions");
        ignored.push_back("what/source");
        ignored.push_back("what/version");
        ignored.push_back("ignore");
        hasher.ignore(ignored);
    }
    
    oh5::hl::HlFile f1, f2, f3;
    SHA1AttributeHasher hasher;
};

// test messages from FIPS PUB 180-1
TEST_F(SHA1AttributeHasher_test, test_fips180_1_sample_1) {
    std::string str("abc");
    std::string hash = SHA1AttributeHasher::sha1hash(str);
    EXPECT_EQ("a9993e364706816aba3e25717850c26c9cd0d89d", hash);
}

TEST_F(SHA1AttributeHasher_test, test_fips180_1_sample_2) {
    std::string str("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    std::string hash = SHA1AttributeHasher::sha1hash(str);
    EXPECT_EQ("84983e441c3bd26ebaae4aa1f95129e5e54670f1", hash);
}

TEST_F(SHA1AttributeHasher_test, test_fips180_1_sample3) {
    std::string str;
    std::string a50 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    str.reserve(1000000);
    for (int i = 0; i < 20000; ++i) {
        str += a50;
    }

    std::string hash = SHA1AttributeHasher::sha1hash(str);
    EXPECT_EQ("34aa973cd4c4daa4f61eeb2bdbad27316534016f", hash);
}

TEST_F(SHA1AttributeHasher_test, attribute_string) {
    oh5::Attribute& a1 = f1.root().create_attribute("a1", oh5::Scalar(1));
    EXPECT_EQ("/a1=1", SHA1AttributeHasher::attribute_string(a1));

    oh5::Group& dataset1 = f1.root().create_group("dataset1");
    oh5::Group& what = dataset1.create_group("what");
    oh5::Attribute& a2 = what.create_attribute("a2", oh5::Scalar(1));
    EXPECT_EQ("/dataset1/what/a2=1", SHA1AttributeHasher::attribute_string(a2));

    a2.value(oh5::Scalar(Date(2000, 12, 13)));
    EXPECT_EQ("/dataset1/what/a2=20001213", SHA1AttributeHasher::attribute_string(a2));

    a2.value(oh5::Scalar(Time(12, 13, 14)));
    EXPECT_EQ("/dataset1/what/a2=121314", SHA1AttributeHasher::attribute_string(a2));
}

TEST_F(SHA1AttributeHasher_test, check_concrete_digests) {
    EXPECT_EQ("0833a94578041a8177afb30ee1e7ac0a660be043", hasher.hash(f1));
    EXPECT_EQ("91176508177e2acc5638faec441a925a268700ae", hasher.hash(f2));

}

TEST_F(SHA1AttributeHasher_test, hash_same_meta) {
    EXPECT_EQ(hasher.hash(f1), hasher.hash(f3));
}

TEST_F(SHA1AttributeHasher_test, hash_different_meta) {
    EXPECT_NE(hasher.hash(f1), hasher.hash(f2));
}

TEST_F(SHA1AttributeHasher_test, hash_ignores_attributes) {
    std::string hash1 = hasher.hash(f1);
    f1.root().create_attribute("ignore", oh5::Scalar("val"));
    std::string hash2 = hasher.hash(f1);
    f1.root().attribute("ignore")->value(oh5::Scalar("val2"));
    std::string hash3 = hasher.hash(f1);

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1, hash3);
    EXPECT_EQ(hash2, hash3);
}

TEST_F(SHA1AttributeHasher_test, hash_changes_when_meta_changes) {
    std::string hash1 = hasher.hash(f1);
    f1.root().create_attribute("attr", oh5::Scalar("val"));
    std::string hash2 = hasher.hash(f1);
    f1.root().attribute("attr")->value(oh5::Scalar("val2"));
    std::string hash3 = hasher.hash(f1);

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash2, hash3);
}

} // namespace brfc
