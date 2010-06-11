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

#include <brfc/ResultSet.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

Variant
ResultSet::value_at(unsigned int pos) const {
    return do_value_at(pos);
}

bool
ResultSet::is_null(unsigned int pos) const {
    return value_at(pos).is_null();
}

String
ResultSet::string(unsigned int pos) const {
    return value_at(pos).to_string();
}

long long
ResultSet::integer(unsigned int pos) const {
    return int64_(pos);
}

long long
ResultSet::int64_(unsigned int pos) const {
    return value_at(pos).to_int64();
}

double
ResultSet::real(unsigned int pos) const {
    return double_(pos);
}

double
ResultSet::double_(unsigned int pos) const {
    return value_at(pos).to_double();
}

bool
ResultSet::boolean(unsigned int pos) const {
    return bool_(pos);
}

bool
ResultSet::bool_(unsigned int pos) const {
    return value_at(pos).to_bool();
}

Date
ResultSet::date(unsigned int pos) const {
    return value_at(pos).to_date();
}

Time
ResultSet::time(unsigned int pos) const {
    return value_at(pos).to_time();
}

} // namespace brfc
