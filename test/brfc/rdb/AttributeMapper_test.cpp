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

#include <brfc/StringList.hpp>

#include <brfc/rdb/AttributeMapper.hpp>

#include <brfc/sql/Table.hpp>
#include <brfc/sql/Column.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_AttributeMapper_test : public testing::Test {
  public:
    rdb_AttributeMapper_test()
            : t1(sql::Table::create("t1"))
            , t2(sql::Table::create("t2"))
            , t3(sql::Table::create("bdb_attribute_values_str"))
            , mapper() {
    }

    void SetUp() {
        t1->add_column("c1");
        t1->add_column("c2");
        t2->add_column("c1");
        t3->add_column("value");
        mapper.add(Mapping(0, "attr1", "string", t1->column("c1")));
        mapper.add(Mapping(1, "attr2", "string", t1->column("c2")));
        mapper.add(Mapping(2, "attr3", "string", t2->column("c1")));
        mapper.add(Mapping(3, "attr4", "string", t3->column("value")));
    }

    sql::TablePtr t1, t2, t3;
    AttributeMapper mapper;
};

TEST_F(rdb_AttributeMapper_test, specializations_on) {
    MappingVector v = mapper.specializations_on(t1);
    ASSERT_EQ(v.size(), (size_t)2);
    EXPECT_EQ(v.at(0).attribute, "attr1");
    EXPECT_EQ(v.at(1).attribute, "attr2");
}

TEST_F(rdb_AttributeMapper_test, is_specialized) {
    EXPECT_TRUE(mapper.is_specialized("attr1"));
    EXPECT_FALSE(mapper.is_specialized("attr4"));
}

TEST_F(rdb_AttributeMapper_test, ignored_in_hash) {
    StringList l = mapper.ignored_in_hash();
    EXPECT_EQ(0, l.size());
    
    mapper.add(Mapping(4, "attr5", "string", t1->column("c1"), true));
    mapper.add(Mapping(5, "attr6", "string", t1->column("c1"), true));

    l = mapper.ignored_in_hash();
    EXPECT_EQ(2, l.size());
    EXPECT_TRUE(l.contains("attr5"));
    EXPECT_TRUE(l.contains("attr6"));
}

} // namespace rdb
} // namespace brfc

