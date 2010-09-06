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
#include <brfc/smart_ptr.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/SHA1AttributeHasher.hpp>

#include <brfc/sql/fwd.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_SHA1AttributeHasher_test : public ::testing::Test {
  public:
    rdb_SHA1AttributeHasher_test()
            : mapper(new AttributeMapper())
            , src()
            , f1(oh5::File::minimal("pvol",
                                    Date(2000, 1, 2),
                                    Time(12, 5),
                                    "WMO:02606"))
            , f2(oh5::File::minimal("pvol",
                                    Date(2001, 1, 2),
                                    Time(12, 5),
                                    "WMO:02606"))
            // same as f1
            , f3(oh5::File::minimal("pvol",
                                    Date(2000, 1, 2),
                                    Time(12, 5),
                                    "WMO:02606")) 
            , hasher(mapper) {
        
    }

    virtual void SetUp() {
        mapper->add(Mapping(1, "Conventions", "string", sql::ColumnPtr(), true));
        mapper->add(Mapping(2, "what/object", "string", sql::ColumnPtr(), false));
        mapper->add(Mapping(3, "what/date", "date", sql::ColumnPtr(), false));
        mapper->add(Mapping(4, "what/time", "time", sql::ColumnPtr(), false));
        mapper->add(Mapping(5, "what/source", "string", sql::ColumnPtr(), true));
        mapper->add(Mapping(6, "what/version", "string", sql::ColumnPtr(), true));
        mapper->add(Mapping(7, "ignore", "string", sql::ColumnPtr(), true));
        mapper->add(Mapping(8, "attr", "string", sql::ColumnPtr(), false));
        src.add("name", "seang");
    }
    
    shared_ptr<AttributeMapper> mapper;
    oh5::Source src;
    shared_ptr<oh5::File> f1, f2, f3;
    SHA1AttributeHasher hasher;
};

TEST_F(rdb_SHA1AttributeHasher_test, missing_source_name_throws) {
    f1->root()->attribute("what/source")->value(Variant("WMO:02606"));
    src.remove("name");
    EXPECT_THROW(hasher.hash(*f1, src), value_error);
}

TEST_F(rdb_SHA1AttributeHasher_test, attribute_string) {
    shared_ptr<oh5::Attribute> a1 = make_shared<oh5::Attribute>("a1", Variant(1));
    EXPECT_EQ("/a1=1", SHA1AttributeHasher::attribute_string(*a1));

    shared_ptr<oh5::DataSetGroup> dataset1 = make_shared<oh5::DataSetGroup>("dataset1");
    shared_ptr<oh5::AttributeGroup> what = make_shared<oh5::AttributeGroup>("what");
    dataset1->add_child(what);
    what->add_child(a1);
    EXPECT_EQ("/dataset1/what/a1=1", SHA1AttributeHasher::attribute_string(*a1));

    a1->value(Variant(Date(2000, 12, 13)));
    EXPECT_EQ("/dataset1/what/a1=20001213", SHA1AttributeHasher::attribute_string(*a1));

    a1->value(Variant(Time(12, 13, 14)));
    EXPECT_EQ("/dataset1/what/a1=121314", SHA1AttributeHasher::attribute_string(*a1));
}

TEST_F(rdb_SHA1AttributeHasher_test, check_concrete_digests) {
    EXPECT_EQ("1f1f260a9a205199a389d57cb20b5fce40d48cf7", hasher.hash(*f1, src));
    EXPECT_EQ("119ac9954c675a020d9901a6e4df43b5c66f6a6c", hasher.hash(*f2, src));
}

TEST_F(rdb_SHA1AttributeHasher_test, hash_same_meta) {
    EXPECT_EQ(hasher.hash(*f1, src), hasher.hash(*f3, src));
}

TEST_F(rdb_SHA1AttributeHasher_test, hash_different_meta) {
    EXPECT_NE(hasher.hash(*f1, src), hasher.hash(*f2, src));
}

TEST_F(rdb_SHA1AttributeHasher_test, hash_ignores_attributes) {
    String hash1 = hasher.hash(*f1, src);
    f1->root()->add_child(make_shared<oh5::Attribute>("ignore", Variant("val")));
    String hash2 = hasher.hash(*f1, src);
    f1->root()->attribute("ignore")->value(Variant("val2"));
    String hash3 = hasher.hash(*f1, src);

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1, hash3);
    EXPECT_EQ(hash2, hash3);
}

TEST_F(rdb_SHA1AttributeHasher_test, hash_changes_when_meta_changes) {
    String hash1 = hasher.hash(*f1, src);
    f1->root()->add_child(make_shared<oh5::Attribute>("attr", Variant("val")));
    String hash2 = hasher.hash(*f1, src);
    f1->root()->attribute("attr")->value(Variant("val2"));
    String hash3 = hasher.hash(*f1, src);

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash2, hash3);
}

} // namespace rdb
} // namespace brfc
