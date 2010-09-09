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
#include <brfc/Database.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/FileEntry.hpp>
#include <brfc/NullStorage.hpp>
#include <brfc/Query.hpp>

#include <brfc/expr/Attribute.hpp>

#include <brfc/oh5/File.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>

namespace brfc {
    
FileCatalog::FileCatalog(const String& dsn,
                         shared_ptr<LocalStorage> storage)
        : db_(new rdb::RelationalDatabase(dsn)) 
        , storage_() {
    this->storage(storage);
    static_pointer_cast<rdb::RelationalDatabase>(db_)->populate_mapper();
}

FileCatalog::FileCatalog(shared_ptr<Database> db,
                         shared_ptr<LocalStorage> storage)
        : db_(db)
        , storage_() {
    this->storage(storage);
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::storage(shared_ptr<LocalStorage> storage) {
    if (not storage)
        storage.reset(new NullStorage());
    storage_ = storage;
}

bool
FileCatalog::is_cataloged(const String& path) const {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
    return is_cataloged(*f);
}

bool
FileCatalog::is_cataloged(const oh5::File& f) const {
    return db_->has_file(f);
}

shared_ptr<const FileEntry>
FileCatalog::catalog(const String& path) {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
    return catalog(*f); 
}

shared_ptr<const FileEntry>
FileCatalog::catalog(const oh5::File& file) {
    shared_ptr<FileEntry> e = db_->save_file(file);
    shared_ptr<const oh5::File> stored_file;
    try {
        stored_file = storage_->prestore(*e);
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    if (stored_file)
        e->file(stored_file);
    return e;
}

bool
FileCatalog::remove(const FileEntry& entry) {
    bool removed = db_->remove_file(entry);
    try {
        storage_->remove(entry);
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::remove: "
                  << e.what() << std::endl << std::flush;
    }
    return removed;
}

Query
FileCatalog::query() const {
    return Query(db_.get());
}

} // namespace brfc
