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

#ifndef BRFC_FILE_CATALOG_HPP
#define BRFC_FILE_CATALOG_HPP

#include <string>

/**
 * @brief main namespace
 */
namespace brfc {
    
    class Database;
    class FileEntry;
    class LocalStorage;

    namespace oh5 {
        class PhysicalFile;
    }
}

namespace brfc {

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
     * @param db database instance (caller retains ownership)
     * @param storage local storage (caller retains ownership)
     */
    explicit
    FileCatalog(Database* db, LocalStorage* storage);

    /**
     * @brief destructor
     */
    ~FileCatalog();
    
    /**
     * @brief access bound Database instance
     */
    Database& database() const {
        return *db_;
    }
    
    /**
     * @brief bind a Database instance
     * @param db the database to bind (caller retains ownership)
     * @throw value_error if db is null
     */
    void database(Database* db);
    
    /**
     * @brief access bound LocalStorage instance
     */
    LocalStorage& storage() const { return *storage_; }
    
    /**
     * @brief bind a LocalStorage instance
     * @param storage the storage to bind (caller retains ownership)
     * @throw value_error if storage is null
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
    bool is_stored(const std::string& path) const;
    
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
     * @note caller takes ownership of the entry
     *
     * this is a short-hand for:
     * @code
     * store(oh5::hl::HlFile(path));
     * @endcode
     *
     * @sa store(oh5::PhysicalFile& file)
     */
    FileEntry* store(const std::string& path);
    
    /**
     * @brief import file to catalog
     * @param file oh5::PhysicalFile instance to import
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw duplicate_entry if file has already been stored
     * @note caller takes ownership of the entry
     * 
     * the FileEntry is passed to LocalStorage::prestore.
     *
     * Exceptions thrown by LocalStorage::prestore are ignored and prestoring
     * is considered failed.
     */
    FileEntry* store(const oh5::PhysicalFile& file);
    
    /**
     * @brief get FileEntry for a file, storing it if necessary
     * @param path absolute path to file
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw fs_error if file can not be opened
     * @note caller takes ownership of the entry
     *
     * this is a short-hand for:
     * @code
     * store(oh5::hl::HlFile(path))
     * @endcode
     */
    FileEntry* get_or_store(const std::string& path);

    /**
     * @brief get FileEntry for a file, storing it if necessary
     * @param file oh5::PhysicalFile instance to get the entry for
     * @return FileEntry instance for the file
     * @throw db_error if storing file to database fails
     * @note caller takes ownership of the entry
     *
     * the FileEntry is passed to LocalStorage::prestore.
     *
     *
     * Exceptions thrown by LocalStorage::prestore are ignored and prestoring
     * is considered failed.
     */
    FileEntry* get_or_store(const oh5::PhysicalFile& file);
    
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
    bool remove(const FileEntry& entry);

    /**
     * @brief absolute local path for database entry idenified by @c uuid
     *
     * equivalent to:
     * @code
     * storage().store(*database().entry_by_uuid(uuid));
     * @endcode
     */
    std::string local_path_for_uuid(const std::string& uuid);
    
  private:
    Database* db_;
    LocalStorage* storage_;
};

} // namespace brfc

#endif // BRFC_FILE_CATALOG_HPP
