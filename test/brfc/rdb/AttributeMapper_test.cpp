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

#include <brfc/rdb/AttributeMapper.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_AttributeMapper_test : public testing::Test {
  public:
    static void SetUpTestCase() {
        mapper.add(Mapping(0, "attr1", "string", "table1", "column1"));
        mapper.add(Mapping(1, "attr2", "string", "table1", "column2"));
        mapper.add(Mapping(2, "attr3", "string", "table2", "column1"));
        mapper.add(Mapping(3, "attr4", "string", "bdb_attribute_values_str", "value"));
    }

    static AttributeMapper mapper;
};

AttributeMapper rdb_AttributeMapper_test::mapper;

TEST_F(rdb_AttributeMapper_test, specializations_on) {
    MappingVector v = mapper.specializations_on("table1");
    ASSERT_EQ(v.size(), (size_t)2);
    EXPECT_EQ(v.at(0).attribute, "attr1");
    EXPECT_EQ(v.at(1).attribute, "attr2");
}

TEST_F(rdb_AttributeMapper_test, is_specialized) {
    EXPECT_TRUE(mapper.is_specialized("attr1"));
    EXPECT_FALSE(mapper.is_specialized("attr4"));
}

} // namespace rdb
} // namespace brfc

