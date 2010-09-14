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

#include <brfc/sql/Dialect.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

String
Dialect::do_variant_to_string(const Variant& value) const {
    switch (value.type()) {
        case Variant::NONE:
            return "NULL";
        case Variant::STRING:
            // XXX: escape!!!
            return "'" + value.string() + "'";
        case Variant::INT64:
        case Variant::DOUBLE:
            return value.to_string();
        case Variant::BOOL:
            return value.to_string().to_lower();
        case Variant::DATE:
            return "'" + value.date().to_string("yyyy-MM-dd") + "'";
        case Variant::TIME:
            return "'" + value.time().to_string("HH:mm:ss.zzz") + "'";
        case Variant::DATETIME:
            return "'" + value.datetime().to_string("yyyy-MM-dd hh:mm:ss.zzz") + "'";
        default:
            throw value_error(value.to_string().to_std_string());
    }
}

} // namespace sql
} // namespace brfc
