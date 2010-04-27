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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/rdb/BindMap.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_BindMap_test : public testing::Test {
  public:
    rdb_BindMap_test() 
            : binds() {
    }

    BindMap binds;
};

TEST_F(rdb_BindMap_test, test_add_qvariant) {
    QVariant val(1);
    binds.add(":bind", val);
    EXPECT_EQ(val, binds.get(":bind"));
}

TEST_F(rdb_BindMap_test, test_add_variant) {
    Variant val(1);
    binds.add(":bind", val);
    EXPECT_EQ(val, binds.get(":bind"));
}

TEST_F(rdb_BindMap_test, test_add_without_colon) {
    QVariant val(1);
    binds.add("bind", val);
    EXPECT_EQ(val, binds.get(":bind"));
}

TEST_F(rdb_BindMap_test, test_add_duplicate) {
    binds.add(":bind", QVariant());
    EXPECT_THROW(binds.add(":bind", QVariant()), duplicate_entry);
}

TEST_F(rdb_BindMap_test, test_get_without_colon) {
    QVariant val(1);
    QVariant result;
    binds.add(":bind", val);
    EXPECT_NO_THROW(result = binds.get("bind"));
    EXPECT_EQ(val, result); 
}

TEST_F(rdb_BindMap_test, test_get_with_default) {
    QVariant default_(1);
    QVariant result;
    EXPECT_NO_THROW(result = binds.get(":bind", default_));
    EXPECT_EQ(default_, result);
}

TEST_F(rdb_BindMap_test, test_size) {
    EXPECT_EQ((size_t)0, binds.size());
    binds.add(":bind", Variant());
    EXPECT_EQ((size_t)1, binds.size());
}

} // namespace rdb
} // namespace brfc

