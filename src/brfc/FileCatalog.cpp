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
#include <brfc/CacheDirStorage.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/db/FileQuery.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/util/BoostFileSystem.hpp>


namespace brfc {

FileCatalog::FileCatalog(db::Database* db, LocalStorage* storage)
        : db_(db)
        , storage_() {
    this->storage(storage);
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::storage(LocalStorage* storage) {
    if (storage == 0)
        storage_.reset(new NullStorage());
    else {
        if (not storage->is_valid()) {
            throw fs_error("invalid LocalStorage");
        }
        storage_.reset(storage, no_delete);
    }
}

bool
FileCatalog::is_stored(const std::string& path) const {
    oh5::hl::HlFile f(path);
    return is_stored(f);
}

bool
FileCatalog::is_stored(const oh5::PhysicalFile& f) const {
    return db_->is_stored(f);
}

db::FileEntry*
FileCatalog::store(const std::string& path) {
    oh5::hl::HlFile f(path);
    return store(f); 
}

db::FileEntry*
FileCatalog::store(const oh5::PhysicalFile& file) {
    auto_ptr<db::FileEntry> e(db_->store(file));
    try {
        storage_->prestore(*e, file.path());
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    return e.release();
}

db::FileEntry*
FileCatalog::get_or_store(const std::string& path) {
    oh5::hl::HlFile f(path);
    return get_or_store(f);
}

db::FileEntry*
FileCatalog::get_or_store(const oh5::PhysicalFile& file) {
    auto_ptr<db::FileEntry> e(db_->get_or_store(file));
    try {
        storage_->prestore(*e, file.path());
    } catch (const std::runtime_error& e) {
        std::cerr << "IGNORED EXCEPTION: LocalStorage::prestore: "
                  << e.what() << std::endl << std::flush;
    }
    return e.release();
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
    return db::FileQuery(db_);
}

db::AttributeQuery
FileCatalog::query_attribute() const {
    return db::AttributeQuery(db_);
}

std::string
FileCatalog::local_path_for_uuid(const std::string& uuid) {
    auto_ptr<db::FileEntry> e(database().entry_by_uuid(uuid));
    return storage().store(*e);
}

} // namespace brfc
