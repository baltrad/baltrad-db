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

#include <brfc/sql/pg/Result.hpp>

#include <pqxx/result>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/pg/Types.hpp>

namespace brfc {
namespace sql {
namespace pg {

Result::Result(const pqxx::result& result,
               const Types* types)
        : result_(new pqxx::result(result))
        , types_(types)
        , row_(-1) {

}

void
Result::do_close() {
    result_.reset();
}

pqxx::result&
Result::result() {
    if (not result_)
        throw db_error("result is closed");
    return *result_;
}

const pqxx::result&
Result::result() const {
    if (not result_)
        throw db_error("result is closed");
    return *result_;
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
Result::do_size() const {
    return result().size();
}

namespace {

Variant
pqtype_to_variant(const pqxx::result::field& field,
                  pqxx::oid coltype,
                  const Types& types) {
    if (field.is_null())
        return Variant();
    else if (coltype == types.int2_oid or
             coltype == types.int4_oid or
             coltype == types.int8_oid)
        return Variant(field.as<long long>());
    else if (coltype == types.float4_oid or
             coltype == types.float8_oid)
        return Variant(field.as<double>());
    else if (coltype == types.bool_oid)
        return Variant(field.as<bool>());
    else if (coltype == types.date_oid)
        return Variant(Date::from_iso_string(field.c_str()));
    else if (coltype == types.time_oid)
        return Variant(Time::from_iso_string(field.c_str()));
    else {
        return Variant(field.c_str());
    }
}

} // namespace anonymous

Variant
Result::do_value_at(unsigned int pos) const {
    try {
        const pqxx::result::field& field = result().at(row_).at(pos);
        pqxx::oid coltype = result().column_type(pos);
        return pqtype_to_variant(field, coltype, *types_);
    } catch (const std::out_of_range&) {
        throw lookup_error("invalid pqxx::result position");
    }
}

Variant
Result::do_value_at(const std::string& col) const  {
    try {
        const pqxx::result::field& field = result().at(row_).at(col);
        pqxx::oid coltype = result().column_type(col);
        return pqtype_to_variant(field, coltype, *types_);
    } catch (const std::invalid_argument&) {
        throw lookup_error("invalid pqxx::result column: " + col);
    }
}

int
Result::do_affected_rows() const {
    return result().affected_rows();
}

} // namespace pg
} // namespace sql
} // namespace brfc
