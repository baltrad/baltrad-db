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

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/ConnectionDtor.hpp>
#include <brfc/sql/ResultProxy.hpp>

namespace brfc {
namespace sql {

ConnectionProxy::ConnectionProxy(Connection* proxied,
                                 shared_ptr<ConnectionDtor> conn_dtor)
        : proxied_(proxied)
        , conn_dtor_(conn_dtor)
        , results_() {
}

ConnectionProxy::~ConnectionProxy() {
    if (proxied_)
        do_close();
}

Connection&
ConnectionProxy::proxied() {
    if (not proxied_)
        throw db_error("ConnectionProxy is closed");
    return *proxied_;
}

const Connection&
ConnectionProxy::proxied() const {
    if (not proxied_)
        throw db_error("ConnectionProxy is closed");
    return *proxied_;
}

void
ConnectionProxy::remove(ResultProxy* result) {
    results_.remove(result);
}

void
ConnectionProxy::do_close() {
    if (proxied_) {
        conn_dtor_->destroy(proxied_);
        proxied_ = 0;
    }
    BOOST_FOREACH(ResultProxy* rp, results_) {
        rp->invalidate();
    }
    results_.clear();
}

bool
ConnectionProxy::do_is_open() const {
    if (proxied_)
        return proxied_->do_is_open();
    return false;
}

void
ConnectionProxy::do_begin() {
    proxied().do_begin();
}

void
ConnectionProxy::do_commit() {
    proxied().do_commit();
}

void
ConnectionProxy::do_rollback() {
    proxied().do_rollback();
}

Result*
ConnectionProxy::do_execute(const std::string& stmt) {
    auto_ptr<Result> r(proxied().do_execute(stmt));
    auto_ptr<ResultProxy> rp(new ResultProxy(this, r.release()));
    results_.push_back(rp.get());
    return rp.release();
}

bool
ConnectionProxy::do_in_transaction() const {
    return proxied().do_in_transaction();
}

const Dialect&
ConnectionProxy::do_dialect() const {
    return proxied().do_dialect();
}

Compiler&
ConnectionProxy::do_compiler() {
    return proxied().do_compiler();
}

void
ConnectionProxy::do_large_object_to_file(long long id, const std::string& path) {
    proxied().do_large_object_to_file(id, path);
}

long long
ConnectionProxy::do_store_large_object(const std::string& path) {
    return proxied().do_store_large_object(path);
}

long long
ConnectionProxy::do_last_insert_id() const {
    return proxied().do_last_insert_id();
}

} // namespace sql
} // namespace brfc
