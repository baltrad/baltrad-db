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

#include <gtest/gtest.h>

#include <brfc/fuse/MockEntry.hpp>
#include <brfc/fuse/NoEntry.hpp>

using ::testing::Return;
using ::testing::ReturnNull;

namespace brfc {
namespace fuse {

class fuse_Entry_test : public ::testing::Test {
  public:
    MockEntry e1, e2, e3;
};

TEST_F(fuse_Entry_test, test_find) {
    EXPECT_CALL(e1, do_child("e2"))
        .WillOnce(Return(&e2));
    
    EXPECT_CALL(e2, do_child("e3"))
        .WillOnce(Return(&e3));
    
    EXPECT_EQ(&e3, &e1.find("/e2/e3"));
}

TEST_F(fuse_Entry_test, test_find_nx) {
    EXPECT_CALL(e1, do_child("e2"))
        .WillOnce(ReturnNull());
    
    Entry& re = e1.find("/e2");
    EXPECT_TRUE(dynamic_cast<NoEntry*>(&re));
}

TEST_F(fuse_Entry_test, test_find_root) {
    EXPECT_EQ(&e1, &e1.find("/"));
}

} // namespace fuse
} // namespace brfc
