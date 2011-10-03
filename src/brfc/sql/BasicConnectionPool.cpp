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

#include <boost/bind.hpp>
#include <boost/checked_delete.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
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
        , size_(0)
        , mutex_()
        , pool_(max_size) {

}

BasicConnectionPool::BasicConnectionPool(const Url& url)
        : creator_(DefaultConnectionCreator(url))
        , size_(0)
        , mutex_()
        , pool_(get_pool_max_size(url)) {
}

BasicConnectionPool::~BasicConnectionPool() {
    BOOST_FOREACH(const LeaseMap::value_type& lease, leased_) {
        lease.second->connection_dtor(boost::checked_deleter<Connection>());
    }
    while (true) {
        try {
            delete pool_.get_nowait();
        } catch (const queue_empty&) {
            break;
        }
    }
}

Connection*
BasicConnectionPool::do_get() {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);

    std::auto_ptr<Connection> conn;
    try {
        conn.reset(pool_.get_nowait());
    } catch (const queue_empty&) {
        conn.reset(create());
    }
    
    std::auto_ptr<ConnectionProxy> proxy(
        new ConnectionProxy(
            conn.release(),
            boost::bind(&BasicConnectionPool::put, this, _1)
        )
    );

    leased_.insert(std::make_pair(&proxy->proxied(), proxy.get()));
    return proxy.release();
}

void
BasicConnectionPool::do_put(Connection* conn) {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);

    if (not conn->is_open()) {
        dispose(conn);
        return;
    }

    try {
        pool_.put_nowait(conn);
    } catch (const queue_full&) {
        dispose(conn);
    }
    leased_.erase(conn);
}

Connection*
BasicConnectionPool::create() {
    if (size_ >= pool_.max_size())
        throw db_error("pool limit reached");
    std::auto_ptr<Connection> c(creator_());
    ++size_;
    return c.release();
}

void
BasicConnectionPool::dispose(Connection* conn) {
    delete conn;
    --size_;
}

} // namespace sql
} // namespace brfc
