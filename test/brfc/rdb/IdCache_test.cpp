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

#include <brfc/rdb/IdCache.hpp>

#include "../common.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::Eq;
using ::testing::ByRef;

namespace brfc {
namespace rdb {

class FakeCached : public enable_shared_from_this<FakeCached> {
  public:
    FakeCached()
            : enable_shared_from_this<FakeCached>() {
    }
};

template<typename T, typename Id>
class MockIdCache : public IdCache<T, Id> {
  public:
    typedef typename IdCache<T, Id>::OptionalId  OptionalId;
    MOCK_METHOD1_T(do_query, OptionalId(const T&));
};

class rdb_IdCache_test : public ::testing::Test {
  public:
    typedef long long Id;
    typedef MockIdCache<FakeCached, Id> Cache;
    typedef Cache::OptionalId OptionalId;

    rdb_IdCache_test()
            : cache() {
    }

    Cache cache;
};

TEST_F(rdb_IdCache_test, test_query_called_if_missing) {
    shared_ptr<FakeCached> obj = make_shared<FakeCached>();
    EXPECT_CALL(cache, do_query(_)).WillOnce(Return(1));

    OptionalId id = cache.get(*obj);
    EXPECT_EQ((size_t)1, cache.size());
    ASSERT_TRUE(id);
    EXPECT_EQ(id.get(), 1);
}

TEST_F(rdb_IdCache_test, test_null_query_not_cached) {
    shared_ptr<FakeCached> obj = make_shared<FakeCached>();
    EXPECT_CALL(cache, do_query(_)).WillOnce(Return(OptionalId()));

    OptionalId id = cache.get(*obj);
    EXPECT_FALSE(id);
    EXPECT_EQ((size_t)0, cache.size());
}

TEST_F(rdb_IdCache_test, test_remove_expired) {
    shared_ptr<FakeCached> obj1 = make_shared<FakeCached>();
    shared_ptr<FakeCached> obj2 = make_shared<FakeCached>();
    cache.set_cached(obj1, 1);
    cache.set_cached(obj2, 2);
    obj1.reset();

    EXPECT_EQ((size_t)2, cache.size());
    cache.remove_expired();
    EXPECT_EQ((size_t)1, cache.size());

    EXPECT_FALSE(cache.get_cached(obj1));
    EXPECT_TRUE(cache.get_cached(obj2));
}

TEST_F(rdb_IdCache_test, test_autoremove_expired_on_get_cached) {
    shared_ptr<FakeCached> obj = make_shared<FakeCached>();
    weak_ptr<FakeCached> weakptr(obj);
    cache.set_cached(weakptr, 1);
    obj.reset();

    OptionalId id = cache.get_cached(weakptr);
    EXPECT_FALSE(id);
    EXPECT_EQ((size_t)0, cache.size());
}

TEST_F(rdb_IdCache_test, test_set_cached_expired_ignored) {
    shared_ptr<FakeCached> obj = make_shared<FakeCached>();
    weak_ptr<FakeCached> weakptr(obj);
    obj.reset();
    cache.set_cached(weakptr, 1);

    EXPECT_EQ((size_t)0, cache.size());
}

TEST_F(rdb_IdCache_test, test_get_cached_nonstored) {
    shared_ptr<FakeCached> obj = make_shared<FakeCached>();
    EXPECT_FALSE(cache.get_cached(obj));
}

} // namespace rdb
} // namespace brfc
