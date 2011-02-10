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

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionCreator.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/PoolReturner.hpp>

namespace brfc {
namespace sql {

BasicConnectionPool::BasicConnectionPool(ConnectionCreator* creator,
                                         int max_size)
        : creator_(creator)
        , returner_(new PoolReturner(this))
        , size_(0)
        , size_mutex_()
        , pool_(max_size) {

}

BasicConnectionPool::~BasicConnectionPool() {
    returner_->pool(0);
    while (true) {
        try {
            delete pool_.get_nowait();
        } catch (const queue_empty&) {
            break;
        }
    }
}

void
BasicConnectionPool::returner(PoolReturner* returner) {
    returner_.reset(returner, no_delete);
    returner_->pool(this);
}

Connection*
BasicConnectionPool::do_get() {
    auto_ptr<Connection> conn;
    try {
        conn.reset(pool_.get_nowait());
    } catch (const queue_empty&) {
        conn.reset(create());
    }
    return new ConnectionProxy(conn.release(), returner_);
}

void
BasicConnectionPool::do_put(Connection* conn) {
    try {
        pool_.put_nowait(conn);
    } catch (const queue_full&) {
        dispose(conn);
    }
}

Connection*
BasicConnectionPool::create() {
    boost::lock_guard<boost::mutex> lock(size_mutex_);
    if (size_ >= pool_.max_size())
        throw db_error("pool limit reached");
    auto_ptr<Connection> c(creator_->create());
    ++size_;
    return c.release();
}

void
BasicConnectionPool::dispose(Connection* conn) {
    boost::lock_guard<boost::mutex> lock(size_mutex_);
    delete conn;
    --size_;
}

} // namespace db
} // namespace brfc
