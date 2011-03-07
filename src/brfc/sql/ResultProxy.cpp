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

#include <brfc/sql/ResultProxy.hpp>
#include <brfc/sql/ConnectionProxy.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

ResultProxy::ResultProxy(ConnectionProxy* conn, Result* result)
        : conn_(conn)
        , result_(result) {

}

ResultProxy::~ResultProxy() {
    if (result_)
        do_close();
}

Result&
ResultProxy::proxied() {
    if (not result_)
        throw db_error("ResultProxy is closed");
    return *result_;
}

const Result&
ResultProxy::proxied() const {
    if (not result_)
        throw db_error("ResultProxy is closed");
    return *result_;
}

void
ResultProxy::invalidate() {
    conn_ = 0;
    do_close();
}

void
ResultProxy::do_close() {
    if (conn_)
        conn_->remove(this);
    if (result_) {
        result_->close();
        result_.reset();
    }
}

bool
ResultProxy::do_next() {
    return proxied().next();
}

bool
ResultProxy::do_seek(int idx) {
    return proxied().seek(idx);
}

int
ResultProxy::do_size() const {
    return proxied().size();
}

Variant
ResultProxy::do_value_at(unsigned int pos) const {
    return proxied().value_at(pos);
}

 Variant
ResultProxy::do_value_at(const std::string& pos) const {
    return proxied().value_at(pos);
}

int
ResultProxy::do_affected_rows() const {
    return proxied().affected_rows();
}

} // namespace sql
} // namespace brfc
