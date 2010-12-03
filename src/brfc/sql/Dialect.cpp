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

#include <boost/algorithm/string/case_conv.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTimeParser.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

std::string
Dialect::do_variant_to_string(const Variant& value) const {
    static DateTimeParser date_parser("yyyy-MM-dd");
    static DateTimeParser time_parser("HH:mm:ss.zzz");
    static DateTimeParser datetime_parser("yyyy-MM-dd hh:mm:ss.zzz");

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
            return boost::to_lower_copy(value.to_string());
        case Variant::DATE:
            return "'" + date_parser.date_to_string(value.date()) + "'";
        case Variant::TIME:
            return "'" + time_parser.time_to_string(value.time()) + "'";
        case Variant::DATETIME:
            return "'" + datetime_parser.to_string(value.datetime()) + "'";
        default:
            throw value_error("could not conv to sql: " + value.to_string());
    }
}

} // namespace sql
} // namespace brfc
