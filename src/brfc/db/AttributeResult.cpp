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

#include <brfc/db/AttributeResult.hpp>

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

Variant
AttributeResult::value_at(const std::string& name) const {
    return do_value_at(name);
}

bool
AttributeResult::is_null(const std::string& name) const {
    return value_at(name).is_null();
}

std::string
AttributeResult::string(const std::string& name) const {
    return value_at(name).to_string();
}

long long
AttributeResult::int64_(const std::string& name) const {
    return value_at(name).to_int64();
}

double
AttributeResult::double_(const std::string& name) const {
    return value_at(name).to_double();
}

bool
AttributeResult::bool_(const std::string& name) const {
    return value_at(name).to_bool();
}

Date
AttributeResult::date(const std::string& name) const {
    return value_at(name).to_date();
}

Time
AttributeResult::time(const std::string& name) const {
    return value_at(name).to_time();
}

} // namespace brfc
