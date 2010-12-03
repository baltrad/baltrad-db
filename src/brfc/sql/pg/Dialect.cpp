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

#include <brfc/sql/pg/Dialect.hpp>

#include <string>

namespace brfc {
namespace sql {
namespace pg {

bool
Dialect::do_has_feature(Feature feature) const {
    switch (feature) {
        case RETURNING:
            // supported since 8.2
            return version_ >= 80200;
        case LAST_INSERT_ID:
            // technically possible, but useless
            // if database supports OIDs, last OID can be fetched
            return false;
        default:
            return false;
    }
}

const std::string&
Dialect::do_name() const {
    return name_;
}

} // namespace pg
} // namespace sql
} // namespace brfc

