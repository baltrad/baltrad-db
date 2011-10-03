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

#include <brfc/sql/ConnectionProxy.hpp>

#include <memory>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionPool.hpp>
#include <brfc/sql/ResultProxy.hpp>

namespace brfc {
namespace sql {

ConnectionProxy::ConnectionProxy(Connection* proxied, ConnectionPool* pool)
        : conn_(proxied)
        , pool_(pool)
        , results_() {
}

ConnectionProxy::~ConnectionProxy() {
    if (conn_)
        do_close();
}

Connection&
ConnectionProxy::conn_ref() {
    if (not conn_)
        throw db_error("ConnectionProxy is closed");
    return *conn_;
}

const Connection&
ConnectionProxy::conn_ref() const {
    if (not conn_)
        throw db_error("ConnectionProxy is closed");
    return *conn_;
}

void
ConnectionProxy::remove(ResultProxy* result) {
    results_.remove(result);
}

void
ConnectionProxy::do_close() {
    BOOST_FOREACH(ResultProxy* rp, results_) {
        rp->invalidate();
    }
    results_.clear();

    if (conn_) {
        pool_->put(conn_);
        conn_ = 0;
        pool_ = 0;
    }
}

bool
ConnectionProxy::do_is_open() const {
    if (conn_)
        return conn_->do_is_open();
    return false;
}

void
ConnectionProxy::do_begin() {
    conn_ref().do_begin();
}

void
ConnectionProxy::do_commit() {
    conn_ref().do_commit();
}

void
ConnectionProxy::do_rollback() {
    conn_ref().do_rollback();
}

Result*
ConnectionProxy::do_execute(const std::string& stmt) {
    std::auto_ptr<Result> r(conn_ref().do_execute(stmt));
    std::auto_ptr<ResultProxy> rp(new ResultProxy(this, r.release()));
    results_.push_back(rp.get());
    return rp.release();
}

bool
ConnectionProxy::do_in_transaction() const {
    return conn_ref().do_in_transaction();
}

const Dialect&
ConnectionProxy::do_dialect() const {
    return conn_ref().do_dialect();
}

Compiler&
ConnectionProxy::do_compiler() {
    return conn_ref().do_compiler();
}

void
ConnectionProxy::do_large_object_to_file(long long id, const std::string& path) {
    conn_ref().do_large_object_to_file(id, path);
}

long long
ConnectionProxy::do_store_large_object(const std::string& path) {
    return conn_ref().do_store_large_object(path);
}

long long
ConnectionProxy::do_last_insert_id() const {
    return conn_ref().do_last_insert_id();
}

} // namespace sql
} // namespace brfc
