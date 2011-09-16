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

#include <brfc/sql/BasicConnectionPool.hpp>

#include <map>
#include <memory>

#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
#include <brfc/sql/PoolReturner.hpp>
#include <brfc/util/Url.hpp>
#include <brfc/util/no_delete.hpp>

namespace brfc {
namespace sql {

namespace {

size_t
get_pool_max_size(const Url& url, size_t value=5) {
    std::map<std::string, std::string> opts = url.http_searchpart();
    if (opts.find("pool_max_size") != opts.end()) {
        try {
            value = boost::lexical_cast<size_t>(opts["pool_max_size"]);
        } catch (const boost::bad_lexical_cast& e) {
            throw std::invalid_argument(
                "invalid value for pool_max_size: " + opts["pool_max_size"]
            );
        }
    }
    return value;
}

} // namespace anonymous

BasicConnectionPool::BasicConnectionPool(ConnectionCreator creator,
                                         int max_size)
        : creator_(creator)
        , returner_(new PoolReturner(this))
        , size_(0)
        , size_mutex_()
        , pool_(max_size) {

}

BasicConnectionPool::BasicConnectionPool(const Url& url)
        : creator_(DefaultConnectionCreator(url))
        , returner_(new PoolReturner(this))
        , size_(0)
        , size_mutex_()
        , pool_(get_pool_max_size(url)) {
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
    std::auto_ptr<Connection> conn;
    try {
        conn.reset(pool_.get_nowait());
    } catch (const queue_empty&) {
        conn.reset(create());
    }
    return new ConnectionProxy(conn.release(), returner_);
}

void
BasicConnectionPool::do_put(Connection* conn) {
    if (not conn->is_open()) {
        dispose(conn);
        return;
    }

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
    std::auto_ptr<Connection> c(creator_());
    ++size_;
    return c.release();
}

void
BasicConnectionPool::dispose(Connection* conn) {
    boost::lock_guard<boost::mutex> lock(size_mutex_);
    delete conn;
    --size_;
}

} // namespace sql
} // namespace brfc
