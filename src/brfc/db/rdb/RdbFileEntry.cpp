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

#include <brfc/db/rdb/RdbFileEntry.hpp>

#include <brfc/assert.hpp>
#include <brfc/String.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/LargeObject.hpp>

namespace brfc {
namespace db {
namespace rdb {

RdbFileEntry::RdbFileEntry(shared_ptr<sql::Connection> conn,
                                         long long id,
                                         long long lo_id)
        : conn_(conn)
        , id_(id)
        , lo_id_(lo_id) {
    BRFC_ASSERT(conn);
}

RdbFileEntry::~RdbFileEntry() {

}

void
RdbFileEntry::do_write_to_file(const String& path) const {
    conn_->begin();
    try {
        conn_->large_object(lo_id_)->write_to_file(path);
        conn_->commit();
    } catch (...) {
        conn_->rollback();
        throw;
    }
}

} // namespace rdb
} // namespace db
} // namespace brfc
