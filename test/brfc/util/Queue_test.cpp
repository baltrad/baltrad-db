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

#include <brfc/util/Queue.hpp>

namespace brfc {

TEST(util_Queue_test, test_put) {
    Queue<int> q;
    q.put(1);
    EXPECT_EQ(1u, q.size());
    q.put(2);
    EXPECT_EQ(2u, q.size());
}

TEST(util_Queue_test, test_put_nonblocking_throws_on_full) {
    Queue<int> q(1);
    q.put(1);
    EXPECT_THROW(q.put(1), queue_full);
}

TEST(util_Queue_test, test_put_timeout_throws_on_full) {
    Queue<int> q(1);
    q.put(1);
    //XXX: should we test the time? it should take >= 25ms
    EXPECT_THROW(q.put(1, 25), queue_full);
}

TEST(util_Queue_test, test_get) {
    Queue<int> q;    
    q.put(1);
    q.put(2);
    EXPECT_EQ(1, q.get());
    EXPECT_EQ(1u, q.size());
    EXPECT_EQ(2, q.get());
    EXPECT_EQ(0u, q.size());
}

TEST(util_Queue_test, test_get_nonblocking_throws_on_empty) {
    Queue<int> q;
    EXPECT_THROW(q.get(), queue_empty);
}

TEST(util_Queue_test, test_get_timeout_throws_on_empty) {
    Queue<int> q;
    //XXX: should we test the time? it should take >= 25ms
    EXPECT_THROW(q.get(25), queue_empty);
}

TEST(util_Queue_test, test_empty) {
    Queue<int> q;
    EXPECT_TRUE(q.empty());
    q.put(1);
    EXPECT_FALSE(q.empty());
}

TEST(util_Queue_test, test_full) {
    Queue<int> q(1);
    EXPECT_FALSE(q.full());
    q.put(1);
    EXPECT_TRUE(q.full());
}

TEST(util_Queue_test, test_full_unlimited) {
    Queue<int> q;
    EXPECT_FALSE(q.full());
}

} // namespace brfc
