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

#include <brfc/sql/LargeObjectProxy.hpp>

namespace brfc {
namespace sql {

LargeObjectProxy::LargeObjectProxy(shared_ptr<Connection> conn,
                                   LargeObject* lobj) 
        : conn_(conn)
        , lobj_(lobj) {

}

LargeObjectProxy::~LargeObjectProxy() {

}

long long
LargeObjectProxy::do_id() const {
    return lobj_->id();
}

void
LargeObjectProxy::do_write_to_file(const std::string& path) const {
    lobj_->write_to_file(path);
}

} // namespace sql
} // namespace brfc
