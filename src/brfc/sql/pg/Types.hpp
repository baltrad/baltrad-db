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

#ifndef BRFC_SQL_PG_TYPES_HPP
#define BRFC_SQL_PG_TYPES_HPP

#include <pqxx/util>

namespace brfc {
namespace sql {
namespace pg {

struct Types {
    pqxx::oid int2_oid;
    pqxx::oid int4_oid;
    pqxx::oid int8_oid;
    pqxx::oid float4_oid;
    pqxx::oid float8_oid;
    pqxx::oid bool_oid;
    pqxx::oid date_oid;
    pqxx::oid time_oid;
};

} // namespace pg
} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_PG_TYPES_HPP
