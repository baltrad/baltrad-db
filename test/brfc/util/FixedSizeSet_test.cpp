/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <gmock/gmock.h>

#include <brfc/util/FixedSizeSet.hpp>

namespace brfc {

class util_FixedSizeSet_test : public ::testing::Test {
  public:
    util_FixedSizeSet_test()
            : set(3)
            , removal_cb() {
    }

    class MockRemovalCb {
      public:
        void operator()(int i) { call(i); }
        MOCK_METHOD1(call, void(int));
    };

    FixedSizeSet<int> set;
    MockRemovalCb removal_cb;
};

TEST_F(util_FixedSizeSet_test, test_ctor_invalid_max_size) {
    EXPECT_THROW(FixedSizeSet<int>(0), std::invalid_argument);
}

TEST_F(util_FixedSizeSet_test, test_ctor_default_overflow) {
    EXPECT_EQ(1u, FixedSizeSet<int>(1).overflow());
    EXPECT_EQ(1u, FixedSizeSet<int>(10).overflow());
    EXPECT_EQ(10u, FixedSizeSet<int>(100).overflow());
}

TEST_F(util_FixedSizeSet_test, test_set_max_size) {
    set.max_size(10);
    EXPECT_EQ(10u, set.max_size());
}

TEST_F(util_FixedSizeSet_test, test_set_max_size_invalid) {
    EXPECT_THROW(set.max_size(0), std::invalid_argument);
}

TEST_F(util_FixedSizeSet_test, test_set_max_size_below_overflow) {
    set.overflow(3);
    set.max_size(2);
    EXPECT_EQ(2u, set.max_size());
    EXPECT_EQ(2u, set.overflow());
}

TEST_F(util_FixedSizeSet_test, test_set_overflow) {
    set.overflow(3);
    EXPECT_EQ(3u, set.overflow());
}

TEST_F(util_FixedSizeSet_test, test_set_overflow_invalid) {
    EXPECT_THROW(set.overflow(4), std::invalid_argument);
    EXPECT_THROW(set.overflow(0), std::invalid_argument);
}

TEST_F(util_FixedSizeSet_test, test_add) {
    set.add(1);
    EXPECT_EQ(1u, set.size());
    EXPECT_TRUE(set.contains(1));
}

TEST_F(util_FixedSizeSet_test, test_add_existing) {
    set.add(1);
    set.add(1);
    EXPECT_EQ(1u, set.size());
}

TEST_F(util_FixedSizeSet_test, test_add_over_limit) {
    set.add(1);
    set.add(2);
    set.add(3);
    EXPECT_EQ(3u, set.size());
    set.add(4);
    EXPECT_EQ(3u, set.size());
    EXPECT_FALSE(set.contains(1));
}

TEST_F(util_FixedSizeSet_test, test_remove) {
    set.add(1);
    set.remove(1);
    EXPECT_FALSE(set.contains(1));
}

TEST_F(util_FixedSizeSet_test, rest_remove_call_removal_callback) {
    set.removal_callback(boost::ref(removal_cb));
    set.add(1);

    EXPECT_CALL(removal_cb, call(1));
    
    set.remove(1);
}

TEST_F(util_FixedSizeSet_test, test_remove_overflow) {
    set.add(1);
    set.add(2);
    set.add(3);
    set.remove_overflow();
    EXPECT_EQ(2u, set.size());
    EXPECT_FALSE(set.contains(1));
    EXPECT_TRUE(set.contains(2));
    EXPECT_TRUE(set.contains(3));
}

TEST_F(util_FixedSizeSet_test, test_remove_overflow_not_reached) {
    set.add(1);
    set.remove_overflow();
    EXPECT_EQ(1u, set.size());
    set.add(2);
    set.remove_overflow();
    EXPECT_EQ(2u, set.size());
}

TEST_F(util_FixedSizeSet_test, test_remove_overflow_calls_removal_callback) {
    set.removal_callback(boost::ref(removal_cb));

    set.add(1);
    set.add(2);
    set.add(3);
    
    EXPECT_CALL(removal_cb, call(1));

    set.add(4);
}

} // namespace brfc
