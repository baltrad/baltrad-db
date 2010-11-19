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

#include <brfc/db/rdb/AttributeMapper.hpp>

#include <brfc/sql/Table.hpp>
#include <brfc/sql/Column.hpp>

#include <brfc/test_common.hpp>

namespace brfc {
namespace db {
namespace rdb {

class db_rdb_AttributeMapper_test : public testing::Test {
  public:
    db_rdb_AttributeMapper_test()
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
        mapper.add(Mapping("attr1", t1->column("c1")));
        mapper.add(Mapping("attr2", t1->column("c2")));
        mapper.add(Mapping("attr3", t2->column("c1")));
        mapper.add(Mapping("attr4", t3->column("value")));
    }

    sql::TablePtr t1, t2, t3;
    AttributeMapper mapper;
};

TEST_F(db_rdb_AttributeMapper_test, specializations_on) {
    MappingVector v = mapper.specializations_on(t1);
    ASSERT_EQ(v.size(), (size_t)2);
    EXPECT_EQ(v.at(0).attribute, "attr1");
    EXPECT_EQ(v.at(1).attribute, "attr2");
}

} // namespace rdb
} // namespace db
} // namespace brfc

