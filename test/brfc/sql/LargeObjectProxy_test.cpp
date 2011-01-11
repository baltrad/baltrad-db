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

#include <brfc/sql/LargeObjectProxy.hpp>

#include <brfc/test_common.hpp>
#include <brfc/sql/MockLargeObject.hpp>
#include <brfc/sql/MockConnection.hpp>

using ::testing::Return;

namespace brfc {
namespace sql {

class sql_LargeObjectProxy_test : public ::testing::Test {
  public:
    sql_LargeObjectProxy_test()
            : lo_ptr(new MockLargeObject())
            , lo(*lo_ptr)
            , conn()
            , proxy(shared_ptr<Connection>(&conn, no_delete),
                    lo_ptr.release()) {
    
    }
    
    auto_ptr<MockLargeObject> lo_ptr;
    MockLargeObject& lo;
    MockConnection conn;
    LargeObjectProxy proxy;
};

TEST_F(sql_LargeObjectProxy_test, test_id) {
    EXPECT_CALL(lo, do_id())
        .WillOnce(Return(10));
    
    EXPECT_EQ(10, proxy.id());
}

TEST_F(sql_LargeObjectProxy_test, test_write_to_file) {
    std::string path = "/path/to/file";
    EXPECT_CALL(lo, do_write_to_file(path));
    
    proxy.write_to_file(path);
}

} // namespace sql
} // namespace brfc
