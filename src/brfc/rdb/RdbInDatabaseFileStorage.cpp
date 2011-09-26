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

#include <brfc/rdb/RdbInDatabaseFileStorage.hpp>

#include <brfc/assert.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>
#include <brfc/rdb/RdbDefaultFileManager.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/sql/Connection.hpp>


namespace brfc {

namespace {

boost::shared_ptr<sql::Connection>
connection(RdbInDatabaseFileStorage& storage) {
    return storage.database()->conn();
}

} // namespace anonymous

RdbInDatabaseFileStorage::RdbInDatabaseFileStorage()
        : db_(0) {

}

RdbInDatabaseFileStorage::~RdbInDatabaseFileStorage() {

}

RelationalDatabase*
RdbInDatabaseFileStorage::database() {
    return db_;
}

void
RdbInDatabaseFileStorage::do_database(RelationalDatabase* db) {
    db_ = db;
}

void
RdbInDatabaseFileStorage::do_store(RdbFileEntry& entry,
                                   const std::string& path) {
    boost::shared_ptr<sql::Connection> conn(connection(*this));
    conn->begin();
    try { 
        RdbDefaultFileManager fmgr(conn);
        long long entry_id = fmgr.insert_file(entry);
        entry.id(entry_id);
        long long lo_id = fmgr.insert_file_content(entry_id, path);
        entry.lo_id(lo_id);
        fmgr.insert_nodes(entry_id, entry.metadata().root());
        conn->commit();
    } catch (...) {
        conn->rollback();
        throw;
    }
}

bool
RdbInDatabaseFileStorage::do_remove(const RdbFileEntry& entry) {
    RdbDefaultFileManager fmgr(connection(*this));
    return fmgr.remove_file(entry.uuid());
}

void
RdbInDatabaseFileStorage::do_retrieve(const RdbFileEntry& entry,
                                      const std::string& path) {
    const long long lo_id = entry.lo_id();
    BRFC_ASSERT(lo_id > 0);
    boost::shared_ptr<sql::Connection> conn(connection(*this));
    conn->begin();
    try {
        conn->large_object_to_file(lo_id, path);
        conn->commit();
    } catch (...) {
        conn->rollback();
        throw;
    }
}

} // namespace brfc
