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
#include <gmock/gmock.h>

#include <brfc/db/rdb/Cache.hpp>

#include "../../common.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::Eq;
using ::testing::ByRef;

namespace brfc {
namespace db {
namespace rdb {

template<typename Key, typename Value>
class MockCache : public Cache<Key, Value> {
  public:
    typedef typename Cache<Key, Value>::OptionalKey OptionalKey;
    typedef typename Cache<Key, Value>::OptionalValue OptionalValue;

    MOCK_METHOD1_T(do_lookup_key, OptionalKey(const Key&));
    MOCK_METHOD1_T(do_lookup_value, OptionalValue(const Value&));
    MOCK_METHOD1_T(do_is_expired, bool(const Value&));
};

class db_rdb_Cache_test : public ::testing::Test {
  public:
    typedef MockCache<int, int> Cache;
    typedef Cache::OptionalKey OptionalKey;
    typedef Cache::OptionalValue OptionalValue;

    db_rdb_Cache_test()
            : cache() {
    }

    Cache cache;
};

TEST_F(db_rdb_Cache_test, test_get_key_LOOKUP_MISSING) {
    EXPECT_CALL(cache, do_lookup_key(1))
        .WillOnce(Return(2));

    OptionalKey key = cache.key(1, Cache::LOOKUP_MISSING);
    EXPECT_EQ((size_t)1, cache.size()); // stored
    ASSERT_TRUE(key);
    EXPECT_EQ(key.get(), 2);
    
    // lookup not called
    key = cache.key(1, Cache::LOOKUP_MISSING);
    ASSERT_TRUE(key);
    EXPECT_EQ(key.get(), 2);
}

TEST_F(db_rdb_Cache_test, test_get_value_LOOKUP_MISSING) {
    EXPECT_CALL(cache, do_lookup_value(1))
        .WillOnce(Return(2));

    OptionalValue value = cache.value(1, Cache::LOOKUP_MISSING);
    EXPECT_EQ((size_t)1, cache.size()); // stored
    ASSERT_TRUE(value);
    EXPECT_EQ(value.get(), 2);
    
    // lookup not called
    value = cache.value(1, Cache::LOOKUP_MISSING);
    ASSERT_TRUE(value);
    EXPECT_EQ(value.get(), 2);
}


TEST_F(db_rdb_Cache_test, test_get_key_FORCE_LOOKUP) {
    EXPECT_CALL(cache, do_lookup_key(1))
        .WillOnce(Return(2))
        .WillOnce(Return(3));

    OptionalKey key = cache.key(1, Cache::FORCE_LOOKUP);
    EXPECT_EQ((size_t)1, cache.size()); // stored
    ASSERT_TRUE(key);
    EXPECT_EQ(key.get(), 2);

    key = cache.key(1, Cache::FORCE_LOOKUP);
    EXPECT_EQ((size_t)1, cache.size());
    ASSERT_TRUE(key);
    EXPECT_EQ(key.get(), 3);
}

TEST_F(db_rdb_Cache_test, test_get_value_FORCE_LOOKUP) {
    EXPECT_CALL(cache, do_lookup_value(1))
        .WillOnce(Return(2))
        .WillOnce(Return(3));

    OptionalValue value = cache.value(1, Cache::FORCE_LOOKUP);
    EXPECT_EQ((size_t)1, cache.size()); // stored
    ASSERT_TRUE(value);
    EXPECT_EQ(value.get(), 2);

    value = cache.value(1, Cache::FORCE_LOOKUP);
    EXPECT_EQ((size_t)1, cache.size());
    ASSERT_TRUE(value);
    EXPECT_EQ(value.get(), 3);
}

TEST_F(db_rdb_Cache_test, test_get_key_NO_LOOKUP) {
    OptionalKey key = cache.key(1, Cache::NO_LOOKUP);
    EXPECT_EQ((size_t)0, cache.size());
    EXPECT_FALSE(key);
}

TEST_F(db_rdb_Cache_test, test_get_value_NO_LOOKUP) {
    OptionalValue value = cache.value(1, Cache::NO_LOOKUP);
    EXPECT_EQ((size_t)0, cache.size());
    EXPECT_FALSE(value);
}

TEST_F(db_rdb_Cache_test, test_empty_key_lookup_not_stored) {
    EXPECT_CALL(cache, do_lookup_key(1))
        .WillOnce(Return(Cache::OptionalKey()));

    OptionalKey key = cache.key(1);
    EXPECT_FALSE(key);
    EXPECT_EQ((size_t)0, cache.size());
}

TEST_F(db_rdb_Cache_test, test_empty_value_lookup_not_stored) {
    EXPECT_CALL(cache, do_lookup_value(1))
        .WillOnce(Return(Cache::OptionalValue()));

    OptionalValue value = cache.value(1);
    EXPECT_FALSE(value);
    EXPECT_EQ((size_t)0, cache.size());    
}

TEST_F(db_rdb_Cache_test, test_get_stored_key_nonstored) {
    EXPECT_FALSE(cache.stored_key(1));
}

TEST_F(db_rdb_Cache_test, test_get_stored_value_nonstored) {
    EXPECT_FALSE(cache.stored_value(1));
}

} // namespace rdb
} // namespace db
} // namespace brfc
