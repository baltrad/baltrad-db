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

#include <brfc/sql/ResultProxy.hpp>

#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

ResultProxy::ResultProxy(shared_ptr<Connection> conn,
                         shared_ptr<Result> result)
        : conn_(conn)
        , result_(result) {

}

ResultProxy::~ResultProxy() {

}

bool
ResultProxy::do_next() {
    return result_->next();
}

bool
ResultProxy::do_seek(int idx) {
    return result_->seek(idx);
}

int
ResultProxy::do_size() const {
    return result_->size();
}

Variant
ResultProxy::do_value_at(unsigned int pos) const {
    return result_->value_at(pos);
}

Variant
ResultProxy::do_value_at(const String& pos) const {
    return result_->value_at(pos);
}

int
ResultProxy::do_affected_rows() const {
    return result_->affected_rows();
}

} // namespace sql
} // namespace brfc
