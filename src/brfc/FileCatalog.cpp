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

#include <brfc/FileCatalog.hpp>

#include <iostream>

#include <brfc/exceptions.hpp>
#include <brfc/LocalStorage.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/Oh5PhysicalFile.hpp>

namespace brfc {

FileCatalog::FileCatalog(Database* db, LocalStorage* storage)
        : db_(0)
        , storage_(0) {
    this->database(db);
    this->storage(storage);
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::database(Database* db) {
    if (not db)
        throw std::invalid_argument("null Database");
    db_ = db;
}

void
FileCatalog::storage(LocalStorage* storage) {
    if (not storage)
        throw std::invalid_argument("null LocalStorage");
    storage_ = storage;
}

bool
FileCatalog::is_stored(const std::string& path) const {
    Oh5PhysicalFile f(path);
    return is_stored(f);
}

bool
FileCatalog::is_stored(const Oh5PhysicalFile& f) const {
    return db_->is_stored(f);
}

FileEntry*
FileCatalog::store(const std::string& path) {
    Oh5PhysicalFile f(path);
    return store(f); 
}

FileEntry*
FileCatalog::store(const Oh5PhysicalFile& file) {
    std::auto_ptr<FileEntry> e(db_->store(file));
    try {
        storage_->prestore(*e, file.path());
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    return e.release();
}

FileEntry*
FileCatalog::get_or_store(const std::string& path) {
    Oh5PhysicalFile f(path);
    return get_or_store(f);
}

FileEntry*
FileCatalog::get_or_store(const Oh5PhysicalFile& file) {
    std::auto_ptr<FileEntry> e(db_->get_or_store(file));
    try {
        storage_->prestore(*e, file.path());
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    return e.release();
}

bool
FileCatalog::remove(const FileEntry& entry) {
    bool removed = db_->remove(entry);
    try {
        storage_->remove(entry);
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::remove: "
                  << e.what() << std::endl << std::flush;
    }
    return removed;
}

std::string
FileCatalog::local_path_for_uuid(const std::string& uuid) {
    std::auto_ptr<FileEntry> e(database().entry_by_uuid(uuid));
    return storage().store(*e);
}

} // namespace brfc
