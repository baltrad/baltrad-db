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

bool
ResultSet::is_null(unsigned int pos) const {
    return do_is_null(pos);
}

String
ResultSet::string(unsigned int pos) const {
    return do_string(pos);
}

long long
ResultSet::integer(unsigned int pos) const {
    return do_int64(pos);
}

long long
ResultSet::int64_(unsigned int pos) const {
    return do_int64(pos);
}

double
ResultSet::real(unsigned int pos) const {
    return do_double(pos);
}

double
ResultSet::double_(unsigned int pos) const {
    return do_double(pos);
}

bool
ResultSet::boolean(unsigned int pos) const {
    return do_bool(pos);
}

bool
ResultSet::bool_(unsigned int pos) const {
    return do_bool(pos);
}

Date
ResultSet::date(unsigned int pos) const {
    return do_date(pos);
}

Time
ResultSet::time(unsigned int pos) const {
    return do_time(pos);
}

} // namespace brfc
