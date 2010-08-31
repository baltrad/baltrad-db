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

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Database.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/Query.hpp>

#include <brfc/expr/Attribute.hpp>

#include <brfc/oh5/File.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>

namespace fs = boost::filesystem;

namespace brfc {
    
FileCatalog::FileCatalog(const String& dsn)
        : db_(new rdb::RelationalDatabase(dsn)) {
    static_pointer_cast<rdb::RelationalDatabase>(db_)->populate_mapper();
}

FileCatalog::FileCatalog(shared_ptr<Database> db)
        : db_(db) {
}

FileCatalog::~FileCatalog() {

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

shared_ptr<FileEntry>
FileCatalog::catalog(const String& path) {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
    return catalog(*f); 
}

shared_ptr<FileEntry>
FileCatalog::catalog(const oh5::File& file) {
    std::string path_utf8 = file.path().to_utf8();
    
    if (is_cataloged(file))
        throw duplicate_entry(path_utf8);
    
    shared_ptr<FileEntry> entry;
    db_->begin();
    // try saving to database
    try {
        entry = db_->save_file(file);
    } catch (const db_error& e) {
        db_->rollback();
        throw;
    }
    db_->commit();
    return entry;
}

void
FileCatalog::remove(const FileEntry& entry) {
    db_->begin();
    try {
        db_->remove_file(entry);
    } catch (const db_error& e) {   
        db_->rollback();
        throw;
    }
    db_->commit();
}

Query
FileCatalog::query() const {
    return Query(db_.get());
}

Query
FileCatalog::query_file_path() const {
    Query q(db_.get());
    q.fetch(expr::Attribute::create("file:path"));
    return q;
}

} // namespace brfc
