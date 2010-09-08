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

#include <brfc/sql/pg/Result.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/pg/Types.hpp>

namespace brfc {
namespace sql {
namespace pg {

Result::Result(const pqxx::result& result,
               const Types* types)
        : result_(result)
        , types_(types)
        , row_(-1) {

}

bool
Result::do_next() {
    ++row_;
    return row_ < size();
}

bool
Result::do_seek(int idx) {
    row_ += idx;
    return row_ < size() and row_ > -1;
}

int
Result::do_size() {
    return result_.size();
}

Variant
Result::pqtype_to_variant(const pqxx::result::field& field,
                                  pqxx::oid coltype) const {
    if (field.is_null())
        return Variant();
    else if (coltype == types_->int2_oid or
             coltype == types_->int4_oid or
             coltype == types_->int8_oid)
        return Variant(field.as<long long>());
    else if (coltype == types_->float4_oid or
             coltype == types_->float8_oid)
        return Variant(field.as<double>());
    else if (coltype == types_->bool_oid)
        return Variant(field.as<bool>());
    else if (coltype == types_->date_oid)
        return Variant(Date::from_extended_iso_string(field.c_str()));
    else if (coltype == types_->time_oid)
        return Variant(Time::from_extended_iso_string(field.c_str()));
    else
        return Variant(String::from_utf8(field.c_str()));
}

Variant
Result::do_value_at(unsigned int pos) const {
    try {
        const pqxx::result::field& field = result_.at(row_).at(pos);
        pqxx::oid coltype = result_.column_type(pos);
        return pqtype_to_variant(field, coltype);
    } catch (const std::out_of_range&) {
        throw lookup_error("invalid pqxx::result position");
    }
}

Variant
Result::do_value_at(const String& col) const  {
    std::string col_std = col.to_std_string();
    try {
        const pqxx::result::field& field = result_.at(row_).at(col_std);
        pqxx::oid coltype = result_.column_type(col_std);
        return pqtype_to_variant(field, coltype);
    } catch (const std::invalid_argument&) {
        throw lookup_error("invalid pqxx::result column");
    }
}

int
Result::do_affected_rows() const {
    return result_.affected_rows();
}

} // namespace pg
} // namespace sql
} // namespace brfc
