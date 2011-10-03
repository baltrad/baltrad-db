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

#include <boost/checked_delete.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionProxy.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
#include <brfc/util/Url.hpp>

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

BasicConnectionPool::BasicConnectionPool(ConnectionCreator conn_creator,
                                         ConnectionDeleter conn_deleter,
                                         int max_size)
        : conn_creator_(conn_creator)
        , conn_deleter_(conn_deleter)
        , max_size_(max_size)
        , mutex_()
        , pool_() {

}

BasicConnectionPool::BasicConnectionPool(const Url& url)
        : conn_creator_(DefaultConnectionCreator(url))
        , conn_deleter_(boost::checked_deleter<Connection>())
        , max_size_(get_pool_max_size(url))
        , mutex_()
        , pool_() {

}

BasicConnectionPool::~BasicConnectionPool() {
    BOOST_FOREACH(const LeaseMap::value_type& lease, leased_) {
        lease.second->close();
    }
    BOOST_FOREACH(Connection* conn, pool_) {
        dispose(conn);
    }
}

size_t
BasicConnectionPool::size() const {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);
    
    return pool_.size() + leased_.size();
}

size_t
BasicConnectionPool::max_size() const {
    return max_size_;
}

size_t
BasicConnectionPool::lease_count() const {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);

    return leased_.size();
}

Connection*
BasicConnectionPool::do_get() {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);
    
    std::auto_ptr<Connection> conn;
    if (not pool_.empty()) {
        conn.reset(pool_.front());
        pool_.pop_front();
    } else {
        conn.reset(create());
    }
    
    std::auto_ptr<ConnectionProxy> proxy(
        new ConnectionProxy(conn.release(), this)
    );

    leased_.insert(std::make_pair(proxy->conn(), proxy.get()));
    return proxy.release();
}

void
BasicConnectionPool::do_put(Connection* conn) {
    boost::lock_guard<boost::recursive_mutex> lock(mutex_);

    leased_.erase(conn);

    if (not conn->is_open() or (size() > max_size_)) {
        dispose(conn);
    } else {
        pool_.push_back(conn);
    }

}

Connection*
BasicConnectionPool::create() {
    if (size() >= max_size_)
        throw db_error("pool limit reached");
    return conn_creator_();
}

void
BasicConnectionPool::dispose(Connection* conn) {
    conn_deleter_(conn);
}

} // namespace sql
} // namespace brfc
