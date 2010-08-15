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
    
FileCatalog::FileCatalog(const String& dsn,
                         const String& storage)
        : db_(new rdb::RelationalDatabase(dsn))
        , namer_(new DefaultFileNamer())
        , storage_(storage) {
    check_storage();
    static_pointer_cast<rdb::RelationalDatabase>(db_)->populate_mapper();
}

FileCatalog::FileCatalog(shared_ptr<Database> db,
                         shared_ptr<FileNamer> namer,
                         const String& storage)
        : db_(db)
        , namer_(namer)
        , storage_(storage) {
    check_storage();
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::check_storage() const {
    std::string path_utf8 = storage_.to_utf8();
    fs::path fs_path = fs::path(path_utf8);
    try {
        if (not fs_path.is_complete())
            throw fs_error("storage '" + path_utf8 + "' is not a complete path");
        if (not fs::exists(fs_path))
            throw fs_error("storage '" + path_utf8 + "' does not exist");
        if (not fs::is_directory(fs_path))
            throw fs_error("storage '" + path_utf8 + "' is not a directory");
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

void
FileCatalog::file_namer(shared_ptr<FileNamer> namer) {
    namer_.swap(namer);
}

void
FileCatalog::file_namer(FileNamer* namer) {
    file_namer(shared_ptr<FileNamer>(namer, no_delete));
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

shared_ptr<oh5::File>
FileCatalog::catalog(const String& path) {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
    catalog(*f); 
    return f;
}

void
FileCatalog::catalog(oh5::File& file) {

    std::string path_utf8 = file.path().to_utf8();
    fs::path fs_path(path_utf8);
    
    file.source(db_->load_source(file.what_source()));

    if (is_cataloged(file))
        throw duplicate_entry(path_utf8);
    
    String proposed_name = namer_->name(file);
    
    // move to FileNamer::name
    if (fs::path(proposed_name.to_utf8()).is_complete())
        throw std::runtime_error("namer must not return absolute paths");
    
    unsigned int name_version = db_->next_filename_version(proposed_name);

    String target = FileNamer::inject_version(proposed_name, name_version);
    fs::path fs_target = fs::path(storage_.to_utf8()) / target.to_utf8();
    String orig_path = file.path();
    // set new path here because we need to store it in db
    file.path(fs_target.string());
    
    db_->begin();
    // try saving to database
    try {
        file.path(fs_target.file_string());
        db_->save_file(file, proposed_name, name_version);
    } catch (const db_error& e) {
        file.path(orig_path);
        db_->rollback();
        throw;
    }

    // database save OK, try copying to new location
    try {
        fs::copy_file(fs_path, fs_target);
        db_->commit();
    } catch (const fs::filesystem_error& e) {
        file.path(orig_path);
        db_->rollback();
        throw fs_error(e.what());
    }
}

void
FileCatalog::remove(const String& path) {
    db_->begin();
    db_->remove_file(path);

    std::string path_utf8 = path.to_utf8();

    try {
        if (not fs::remove(fs::path(path_utf8)))
            throw fs_error("file '" + path_utf8 + "' does not exist");
        db_->commit();
    } catch (fs::filesystem_error& e) {
        db_->rollback();
        throw fs_error(e.what());
    } catch (fs_error) {
        db_->rollback();
        throw;
    }
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
