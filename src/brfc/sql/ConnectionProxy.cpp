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

#include <brfc/sql/ConnectionProxy.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/ConnectionPool.hpp>
#include <brfc/sql/ResultProxy.hpp>

namespace brfc {
namespace sql {

ConnectionProxy::ConnectionProxy(ConnectionPool* pool, Connection* proxied)
        : pool_(pool)
        , proxied_(proxied) {
}

shared_ptr<ConnectionProxy>
ConnectionProxy::create(ConnectionPool* pool, Connection* proxied) {
    return shared_ptr<ConnectionProxy>(new ConnectionProxy(pool, proxied));
}

ConnectionProxy::~ConnectionProxy() {
    if (pool_)
        pool_->put(proxied_.release());
}

void
ConnectionProxy::release() {
    pool_ = 0;
}

void
ConnectionProxy::do_open() {
    proxied_->do_open();
}

void
ConnectionProxy::do_close() {
    proxied_->do_close();
}

bool
ConnectionProxy::do_is_open() const {
    return proxied_->do_is_open();
}

void
ConnectionProxy::do_begin() {
    proxied_->do_begin();
}

void
ConnectionProxy::do_commit() {
    proxied_->do_commit();
}

void
ConnectionProxy::do_rollback() {
    proxied_->do_rollback();
}

Result*
ConnectionProxy::do_execute(const std::string& stmt) {
    auto_ptr<Result> r(proxied_->do_execute(stmt));
    return new ResultProxy(shared_from_this(), r.release());
}

bool
ConnectionProxy::do_in_transaction() const {
    return proxied_->do_in_transaction();
}

const Dialect&
ConnectionProxy::do_dialect() const {
    return proxied_->do_dialect();
}

Compiler&
ConnectionProxy::do_compiler() {
    return proxied_->do_compiler();
}

void
ConnectionProxy::do_large_object_to_file(long long id, const std::string& path) {
    proxied_->do_large_object_to_file(id, path);
}

long long
ConnectionProxy::do_store_large_object(const std::string& path) {
    return proxied_->do_store_large_object(path);
}

long long
ConnectionProxy::do_last_insert_id() const {
    return proxied_->do_last_insert_id();
}

} // namespace sql
} // namespace brfc
