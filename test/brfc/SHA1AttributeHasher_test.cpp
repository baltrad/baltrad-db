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

#include <brfc/smart_ptr.hpp>
#include <brfc/SHA1AttributeHasher.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include "common.hpp"

namespace brfc {

class SHA1AttributeHasher_test : public ::testing::Test {
  public:
    SHA1AttributeHasher_test()
            : specs(new oh5::AttributeSpecs())
            , src(make_shared<oh5::SourceRadar>())
            , f1(oh5::File::minimal("pvol",
                                    QDate(2000, 1, 2),
                                    QTime(12, 5),
                                    "WMO:02606"))
            , f2(oh5::File::minimal("pvol",
                                    QDate(2001, 1, 2),
                                    QTime(12, 5),
                                    "WMO:02606"))
            // same as f1
            , f3(oh5::File::minimal("pvol",
                                    QDate(2000, 1, 2),
                                    QTime(12, 5),
                                    "WMO:02606")) 
            , hasher(specs) {
        
    }

    virtual void SetUp() {
        specs->add(oh5::AttributeSpec("Conventions", "string", true));
        specs->add(oh5::AttributeSpec("what/object", "string", false));
        specs->add(oh5::AttributeSpec("what/date", "date", false));
        specs->add(oh5::AttributeSpec("what/time", "time", false));
        specs->add(oh5::AttributeSpec("what/source", "string", true));
        specs->add(oh5::AttributeSpec("what/version", "string", true));
        specs->add(oh5::AttributeSpec("ignore", "string", true));
        specs->add(oh5::AttributeSpec("attr", "string", false));
        f1->source(src);
        f2->source(src);
        f3->source(src);
    }
    
    shared_ptr<oh5::AttributeSpecs> specs;
    shared_ptr<oh5::SourceRadar> src;
    shared_ptr<oh5::File> f1, f2, f3;
    SHA1AttributeHasher hasher;
};

TEST_F(SHA1AttributeHasher_test, hash_same_meta) {
    EXPECT_EQ(hasher.hash(*f1), hasher.hash(*f3));
}

TEST_F(SHA1AttributeHasher_test, hash_different_meta) {
    EXPECT_NE(hasher.hash(*f1), hasher.hash(*f2));
}

TEST_F(SHA1AttributeHasher_test, hash_ignores_attributes) {
    QString hash1 = hasher.hash(*f1);
    f1->root()->add_child(make_shared<oh5::Attribute>("ignore", Variant("val")));
    QString hash2 = hasher.hash(*f1);
    f1->root()->attribute("ignore")->value(Variant("val2"));
    QString hash3 = hasher.hash(*f1);

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1, hash3);
    EXPECT_EQ(hash2, hash3);
}

TEST_F(SHA1AttributeHasher_test, hash_changes_when_meta_changes) {
    QString hash1 = hasher.hash(*f1);
    f1->root()->add_child(make_shared<oh5::Attribute>("attr", Variant("val")));
    QString hash2 = hasher.hash(*f1);
    f1->root()->attribute("attr")->value(Variant("val2"));
    QString hash3 = hasher.hash(*f1);

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash2, hash3);
}


} // namespace brfc
