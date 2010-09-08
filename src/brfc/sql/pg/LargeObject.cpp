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

#include <brfc/sql/pg/LargeObject.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <brfc/String.hpp>

namespace brfc {
namespace sql {
namespace pg {

LargeObject::LargeObject(pqxx::dbtransaction& tx, long long id)
        : lob_(tx, boost::numeric_cast<pqxx::oid>(id)) {
    
}

LargeObject::LargeObject(pqxx::dbtransaction& tx, const String& path)
        : lob_(tx, path.to_utf8()) {

}

long long
LargeObject::do_id() const {
    return boost::numeric_cast<long long>(lob_.id());
}

void
LargeObject::do_write_to_file(const String& path) const {
    lob_.to_file(path.to_utf8());
}

} // namespace pg
} // namespace sql
} // namespace brfc
