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

#include <brfc/rdb/PostgresResultSet.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace rdb {

PostgresResultSet::PostgresResultSet(const pqxx::result& result)
        : result_(result)
        , row_(-1) {

}

bool
PostgresResultSet::do_next() {
    ++row_;
    return row_ < size();
}

bool
PostgresResultSet::do_seek(int idx) {
    row_ += idx;
    return row_ < size() and row_ > -1;
}

int
PostgresResultSet::do_size() {
    return result_.size();
}

Variant
PostgresResultSet::do_value_at(unsigned int pos) const {
    try {
        const pqxx::result::field& field = result_.at(row_).at(pos);
        if (field.is_null()) {
            return Variant();
        } else {
            return Variant(String::from_utf8(field.c_str()));
        }
    } catch (const std::out_of_range&) {
        throw lookup_error("invalid pqxx::result position");
    }
}

} // namespace rdb
} // namespace brfc
