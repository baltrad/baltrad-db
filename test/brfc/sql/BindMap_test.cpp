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
#include <brfc/sql/BindMap.hpp>

#include <brfc/test_common.hpp>

namespace brfc {
namespace sql {

class sql_BindMap_test : public testing::Test {
  public:
    sql_BindMap_test() 
            : binds() {
    }

    BindMap binds;
};

TEST_F(sql_BindMap_test, test_add_variant) {
    Variant val(1);
    binds.add(":bind", val);
    EXPECT_EQ(val, binds.get(":bind"));
}

TEST_F(sql_BindMap_test, test_add_without_colon) {
    Variant val(1);
    binds.add("bind", val);
    EXPECT_EQ(val, binds.get(":bind"));
}

TEST_F(sql_BindMap_test, test_add_duplicate) {
    binds.add(":bind", Variant());
    EXPECT_THROW(binds.add(":bind", Variant()), duplicate_entry);
}

TEST_F(sql_BindMap_test, test_get_without_colon) {
    Variant val(1);
    Variant result;
    binds.add(":bind", val);
    EXPECT_NO_THROW(result = binds.get("bind"));
    EXPECT_EQ(val, result); 
}

TEST_F(sql_BindMap_test, test_get_with_default) {
    Variant default_(1);
    Variant result;
    EXPECT_NO_THROW(result = binds.get(":bind", default_));
    EXPECT_EQ(default_, result);
}


TEST_F(sql_BindMap_test, test_set) {
    EXPECT_THROW(binds.set(":bind", Variant()), lookup_error);
    
    binds.add(":bind", Variant());
    EXPECT_NO_THROW(binds.set(":bind", Variant(1)));
    ASSERT_TRUE(binds.has(":bind"));
    EXPECT_EQ(Variant(1), binds.get(":bind"));
}

TEST_F(sql_BindMap_test, test_set_without_colon) {
    EXPECT_THROW(binds.set("bind", Variant()), lookup_error);
    
    binds.add(":bind", Variant());
    EXPECT_NO_THROW(binds.set("bind", Variant(1)));
    ASSERT_TRUE(binds.has(":bind"));
    EXPECT_EQ(Variant(1), binds.get(":bind"));
}


TEST_F(sql_BindMap_test, test_size) {
    EXPECT_EQ((size_t)0, binds.size());
    binds.add(":bind", Variant());
    EXPECT_EQ((size_t)1, binds.size());
}

TEST_F(sql_BindMap_test, test_copy) {
    Variant val(1);
    binds.add(":bind", val);

    BindMap copy1(binds);
    EXPECT_EQ(binds.size(), copy1.size());
    EXPECT_EQ(val, copy1.get(":bind", Variant()));

    BindMap copy2;
    copy2 = binds;
    EXPECT_EQ(binds.size(), copy2.size());
    EXPECT_EQ(val, copy2.get(":bind", Variant()));

    binds = binds;
    EXPECT_EQ((size_t)1, binds.size());
    EXPECT_EQ(val, binds.get(":bind", Variant()));
}

TEST_F(sql_BindMap_test, test_remove) {
    Variant val(1);
    binds.add(":bind1", val);
    binds.add(":bind2", val);
    
    EXPECT_FALSE(binds.remove(":nxbind"));

    EXPECT_TRUE(binds.remove(":bind1"));
    EXPECT_TRUE(binds.remove("bind2"));
    EXPECT_EQ(binds.size(), (size_t)0);
    EXPECT_FALSE(binds.has(":bind1"));
    EXPECT_FALSE(binds.has(":bind2"));
}

TEST_F(sql_BindMap_test, test_clear) {
    Variant val(1);
    binds.add(":bind1", val);
    binds.add(":bind2", val);

    binds.clear();
    EXPECT_EQ(binds.size(), (size_t)0);
    EXPECT_FALSE(binds.has(":bind1"));
    EXPECT_FALSE(binds.has(":bind2"));
}

} // namespace sql
} // namespace brfc

