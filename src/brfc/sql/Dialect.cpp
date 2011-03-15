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

#include <brfc/sql/Dialect.hpp>

#include <sstream>

#include <boost/algorithm/string/case_conv.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

std::string
ansi_sql_interval_str(const TimeDelta& delta) {
    std::stringstream ss;
    ss.fill('0');

    int msecs = delta.msecs();
    ss << delta.days() << " ";
    ss.width(2);
    ss << msecs / MSECS_IN_HOUR << ":";
    msecs %= MSECS_IN_HOUR;
    ss.width(2);
    ss << msecs / MSECS_IN_MIN << ":";
    msecs %= MSECS_IN_MIN;
    ss.width(2);
    ss << msecs / MSECS_IN_SEC << ".";
    msecs %= MSECS_IN_SEC;
    ss.width(3);
    ss << msecs; // msec
    
    return ss.str();
}

std::string
Dialect::do_variant_to_string(const Variant& value) const {
    switch (value.type()) {
        case Variant::NONE:
            return "NULL";
        case Variant::STRING:
            return "'" + escape(value.string()) + "'";
        case Variant::INT64:
        case Variant::DOUBLE:
            return value.to_string();
        case Variant::BOOL:
            return boost::to_lower_copy(value.to_string());
        case Variant::DATE:
            return "'" + value.date().to_iso_string(true) + "'";
        case Variant::TIME:
            return "'" + value.time().to_iso_string(true) + "'";
        case Variant::DATETIME:
            return "'" + value.datetime().to_iso_string(true) + "'";
        case Variant::TIMEDELTA:
            return "INTERVAL '" + ansi_sql_interval_str(value.timedelta()) +  "'";
        default:
            throw value_error("could not conv to sql: " + value.to_string());
    }
}

} // namespace sql
} // namespace brfc
