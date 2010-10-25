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

#include <brfc/FileCatalog.hpp>

#include <iostream>

#include <brfc/exceptions.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/NullStorage.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/oh5/hl/HlFile.hpp>


namespace brfc {
    
FileCatalog::FileCatalog(const String& dsn, LocalStorage* storage)
        : db_(new db::rdb::RelationalDatabase(dsn)) 
        , storage_() {
    this->storage(storage);
    static_pointer_cast<db::rdb::RelationalDatabase>(db_)->populate_mapper();
}

FileCatalog::FileCatalog(db::Database* db, LocalStorage* storage)
        : db_(db, no_delete)
        , storage_() {
    this->storage(storage);
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::storage(LocalStorage* storage) {
    if (storage == 0)
        storage_.reset(new NullStorage());
    else
        storage_.reset(storage, no_delete);
}

bool
FileCatalog::is_stored(const String& path) const {
    return is_stored(oh5::hl::HlFile(path));
}

bool
FileCatalog::is_stored(const oh5::PhysicalFile& f) const {
    return db_->is_stored(f);
}

shared_ptr<const db::FileEntry>
FileCatalog::store(const String& path) {
    return store(oh5::hl::HlFile(path)); 
}

shared_ptr<const db::FileEntry>
FileCatalog::store(const oh5::PhysicalFile& file) {
    shared_ptr<db::FileEntry> e = db_->store(file);
    try {
        storage_->prestore(*e, file.path());
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    return e;
}

bool
FileCatalog::remove(const db::FileEntry& entry) {
    bool removed = db_->remove(entry);
    try {
        storage_->remove(entry);
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::remove: "
                  << e.what() << std::endl << std::flush;
    }
    return removed;
}

db::FileQuery
FileCatalog::query_file() const {
    return db::FileQuery(db_.get());
}

db::AttributeQuery
FileCatalog::query_attribute() const {
    return db::AttributeQuery(db_.get());
}

} // namespace brfc
