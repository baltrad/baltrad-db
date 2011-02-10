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

#include <brfc/sql/BasicConnectionPool.hpp>

#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>

#include <brfc/sql/ConnectionCreator.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/PoolReturner.hpp>

namespace brfc {
namespace sql {

BasicConnectionPool::BasicConnectionPool(ConnectionCreator* creator)
        : creator_(creator)
        , returner_(new PoolReturner(this))
        , lock_()
        , pool_() {

}

BasicConnectionPool::BasicConnectionPool(ConnectionCreator* creator,
                                         PoolReturner* returner)
        : creator_(creator)
        , returner_(returner, no_delete)
        , lock_()
        , pool_() {
    returner->pool(this);
}


BasicConnectionPool::~BasicConnectionPool() {
    returner_->pool(0);
}

Connection*
BasicConnectionPool::do_get() {
    boost::lock_guard<boost::mutex> guard(lock_);
    std::auto_ptr<Connection> conn;
    if (pool_.size() != 0) {
        conn.reset(pool_.pop_back().release());
    } else {
        conn.reset(creator_->create());
    }
    return new ConnectionProxy(conn.release(), returner_);
}

void
BasicConnectionPool::do_put(Connection* conn) {
    boost::lock_guard<boost::mutex> guard(lock_);
    pool_.push_back(conn);
}

} // namespace db
} // namespace brfc
