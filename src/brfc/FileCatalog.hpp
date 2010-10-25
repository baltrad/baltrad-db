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

#ifndef BRFC_FILE_CATALOG_HPP
#define BRFC_FILE_CATALOG_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/String.hpp>

/**
 * @brief main namespace
 */
namespace brfc {
    
class LocalStorage;

namespace db {

class AttributeQuery;
class Database;
class FileEntry;
class FileQuery;

} // namespace db

namespace oh5 {
    class PhysicalFile;
}

/**
 * @brief indexes and stores ODIM_H5 File instances
 *
 * @note there are several similar methods accepting file as as a path string
 *       or oh5::PhysicalFile instance. Methods accepting file path are mostly
 *       just short-hands for loading the file from filesystem and then passing
 *       it to the method accepting oh5::PhysicalFile instance. If you want to
 *       call several methods on the same file, it's more efficient to create
 *       the oh5::PhysicalFile instance yourself.
 */
class FileCatalog {
  public:
    /**
     * @brief constructor
     * @param dsn database connection string (using URL-like syntax)
     * @param storage local storage (caller retains ownership)
     *
     * @throw db_error if DB could not be opened
     * @throw value_error if dsn is invalid
     */
    explicit
    FileCatalog(const String& dsn, LocalStorage* storage=0);

    /**
     * @brief constructor
     * @param db database instance (caller retains ownership)
     * @param storage local storage (caller retains ownership)
     */
    explicit
    FileCatalog(db::Database* db, LocalStorage* storage=0);
    
    /**
     * @brief destructor
     */
    ~FileCatalog();
    
    /**
     * @brief access bound Database instance
     */
    db::Database& database() {
        return *db_;
    }
    
    /**
     * @brief access bound LocalStorage instance
     */
    LocalStorage& storage() { return *storage_; }
    
    /**
     * @brief bind a LocalStorage instance
     * @param storage local storage (caller retains ownership)
     *
     * if @c storage is null, bind NullStorage
     */
    void storage(LocalStorage* storage);

    /**
     * @brief has file been stored in this catalog
     * @param path - absolute path to file
     * @throw db_error if database query fails
     * @throw fs_error if file can not be opened
     * @return true if file is stored
     * 
     * this is a short-hand for:
     * @code
     * is_stored(oh5::hl::HlFile(path));
     * @endcode
     *
     * @sa File::unique_identifier
     * @sa is_stored(const oh5::PhysicalFile&) const
     */
    bool is_stored(const String& path) const;
    
    /**
     * @brief has file been imported to this catalog
     * @param file oh5::PhysicalFile instance to test
     * @throw db_error if database query fails
     * @return true if file is stored
     *
     * a file is stored when a file with the same unique identifier is
     * already present in database. For the exact rules on how the unique id
     * is determined, refer to File documentation.
     */
    bool is_stored(const oh5::PhysicalFile& f) const;
    
    /**
     * @brief import file to catalog
     * @param path absolute path to file
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw fs_error if file can not be opened
     * @throw duplicate_entry if file has already been stored
     *
     * this is a short-hand for:
     * @code
     * store(oh5::hl::HlFile(path));
     * @endcode
     *
     * @sa store(oh5::PhysicalFile& file)
     */
    shared_ptr<const db::FileEntry> store(const String& path);
    
    /**
     * @brief import file to catalog
     * @param file oh5::PhysicalFile instance to import
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw duplicate_entry if file has already been stored
     * 
     * the FileEntry is passed to LocalStorage::prestore and if successful,
     * the resulting oh5::PhysialFile will replace FileEntry::file.
     *
     * Exceptions thrown by LocalStorage::prestore are ignored and prestoring
     * is considered failed.
     */
    shared_ptr<const db::FileEntry> store(const oh5::PhysicalFile& file);
    
    /**
     * @brief remove file from catalog
     * @param entry database entry
     * @return true if file was removed, false if it did not exist
     * @throw db_error if removing file entry from database fails
     * @throw fs_error if removing file entry from filesystem fails
     *
     * the FileEntry is passed to LocalStorage::remove.
     *
     * Exceptions thrown by LocalStorage::remove are ignored and removing from
     * local storage is considered failed.
     */
    bool remove(const db::FileEntry& entry);

    /**
     * @brief get a query object bound to owned database
     */
    db::FileQuery query_file() const;

    db::AttributeQuery query_attribute() const;
    
  private:
    shared_ptr<db::Database> db_;
    shared_ptr<LocalStorage> storage_;
};

}

#endif // BRFC_FILE_CATALOG_HPP
