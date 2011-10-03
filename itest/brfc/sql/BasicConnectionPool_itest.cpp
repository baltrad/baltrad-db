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

#include <boost/thread.hpp>

#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/FakeConnection.hpp>
#include <brfc/util/no_delete.hpp>

namespace brfc {
namespace sql {

namespace {

Connection*
create_connection() {
    return new FakeConnection();
}

class connection_access {
  public:
    connection_access(sql::ConnectionPool* pool, int checkout_count)
            : pool_(pool)
            , checkout_count_(checkout_count) {
    }

    void operator()() {
        for (int i = 0; i < checkout_count_; ++i) {
            std::auto_ptr<Connection> conn(pool_->get());
        }
    }

  private:
    sql::ConnectionPool* pool_;
    const int checkout_count_;
};

} // namespace anonymous

TEST(BasicConnectionPool_itest, test_thread_safety) {
    
    BasicConnectionPool pool(create_connection, no_delete);

    const int THREAD_COUNT = 2;
    const int CHECKOUT_COUNT = 500;

    boost::thread_group g;
    for (int i=0; i < THREAD_COUNT; ++i) {
        g.create_thread(connection_access(&pool, CHECKOUT_COUNT));
    }
    g.join_all();

}

} // namespace sql
} // namespace brfc
