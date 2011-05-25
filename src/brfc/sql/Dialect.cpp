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
#include <stdexcept>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>

#include <brfc/expr/Expression.hpp>

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
Dialect::do_literal_to_string(const Expression& x) const {
    std::vector<std::string> strs;
    switch (x.type()) {
        case Expression::type::LIST:
            if (x.empty())
                return "NULL";
            BOOST_FOREACH(const Expression& e, x) {
                strs.push_back(literal_to_string(e));
            }
            return "(" + boost::join(strs, ", ") + ")";
        case Expression::type::BOOL:
            return x.bool_() ? "true" : "false";
        case Expression::type::INT64:
            return boost::lexical_cast<std::string>(x.int64());
        case Expression::type::DOUBLE:
            return boost::lexical_cast<std::string>(x.double_());
        case Expression::type::STRING:
            return "'" + escape(x.string()) + "'";
        case Expression::type::DATE:
            return "'" + x.date().to_iso_string(true) + "'";
        case Expression::type::TIME:
            return "'" + x.time().to_iso_string(true) + "'";
        case Expression::type::DATETIME:
            return "'" + x.datetime().to_iso_string(true) + "'";
        case Expression::type::INTERVAL:
            return "INTERVAL '" + ansi_sql_interval_str(x.interval()) + "'";
        default:
            throw std::logic_error("invalid literal:");
    }
}

} // namespace sql
} // namespace brfc
