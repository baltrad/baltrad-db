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

#include <brfc/db/AttributeResult.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace db {

Variant
AttributeResult::value_at(unsigned int pos) const {
    return do_value_at(pos);
}

bool
AttributeResult::is_null(unsigned int pos) const {
    return value_at(pos).is_null();
}

String
AttributeResult::string(unsigned int pos) const {
    return value_at(pos).to_string();
}

long long
AttributeResult::int64_(unsigned int pos) const {
    return value_at(pos).to_int64();
}

double
AttributeResult::double_(unsigned int pos) const {
    return value_at(pos).to_double();
}

bool
AttributeResult::bool_(unsigned int pos) const {
    return value_at(pos).to_bool();
}

Date
AttributeResult::date(unsigned int pos) const {
    return value_at(pos).to_date();
}

Time
AttributeResult::time(unsigned int pos) const {
    return value_at(pos).to_time();
}

} // namespace db
} // namespace brfc
