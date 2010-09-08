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

#ifndef BRFC_FILE_CATALOGER_H
#define BRFC_FILE_CATALOGER_H

#include <brfc/smart_ptr.hpp>

#include <brfc/String.hpp>

/**
 * @brief main namespace
 */
namespace brfc {
    
class Database;
class FileEntry;
class Query;

namespace oh5 {
    class File;
}

/**
 * @brief indexes and stores ODIM_H5 File instances
 *
 * @note there are several similar methods accepting file as as a path string
 *       or oh5::File instance. Methods accepting file path are mostly just
 *       short-hands for loading the file from filesystem and then passing
 *       it to the method accepting oh5::File instance. If you want to call
 *       several methods on the same file, it's more efficient to create the
 *       oh5::File instance yourself.
 */
class FileCatalog {
  public:
    /**
     * @brief constructor
     * @param dsn database connection string (using URL-like syntax)
     *
     * @throw db_error if DB could not be opened
     * @throw value_error if dsn is invalid
     */
    FileCatalog(const String& dsn);
    
    /**
     * @brief constructor
     * @param db database instance
     */
    FileCatalog(shared_ptr<Database> db);

    /**
     * @brief destructor
     */
    ~FileCatalog();
    
    /**
     * @brief access bound database instance
     */
    shared_ptr<Database> database() {
        return db_;
    }

    /**
     * @brief has file been imported to this catalog
     * @param path - absolute path to file
     * @throw db_error if database query fails
     * @throw fs_error if file can not be opened
     * @return true if file is cataloged
     * 
     * this is a short-hand for:
     * @code
     * FileCatalog fc(...);
     * shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
     * fc.is_cataloged(*f);
     * @endcode
     *
     * @sa File::unique_identifier
     * @sa is_cataloged(const oh5::File&) const
     */
    bool is_cataloged(const String& path) const;
    
    /**
     * @brief has file been imported to this catalog
     * @param file oh5::File instance to test
     * @throw db_error if database query fails
     * @return true if file is cataloged
     *
     * a file is cataloged when a file with the same unique identifier is
     * already present in database. For the exact rules on how the unique id
     * is determined, refer to File documentation.
     */
    bool is_cataloged(const oh5::File& f) const;
    
    /**
     * @brief import file to catalog
     * @param path absolute path to file
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw fs_error if file can not be opened
     * @throw duplicate_entry if file has already been cataloged
     *
     * this is a short-hand for:
     * @code
     * FileCatalog fc(...);
     * shared_ptr<oh5::File> f = oh5::File::from_filesystem(path);
     * fc.catalog(*f);
     * @endcode
     *
     * @sa catalog(oh5::File& file)
     */
    shared_ptr<FileEntry> catalog(const String& path);
    
    /**
     * @brief import file to catalog
     * @param file oh5::File instance to import
     * @return FileEntry instance of the stored file
     * @throw db_error if storing file to database fails
     * @throw duplicate_entry if file has already been cataloged
     *
     * on import file is physically copied to a new location. If the file
     * import is successful file.path() will contain this new location.
     */
    shared_ptr<FileEntry> catalog(const oh5::File& file);
    
    /**
     * @brief remove file from catalog
     * @param entry database entry
     * @return true if file was removed, false if it did not exist
     * @throw db_error if removing file entry from database fails
     * @throw fs_error if removing file entry from filesystem fails
     */
    bool remove(const FileEntry& entry);

    /**
     * @brief get a query object bound to owned database
     */
    Query query() const;
    
    /**
     * @brief get a query object bound to owned database
     *
     * shorthand for
     *
     * @code
     * Query q = fc.query();
     * q.fetch(expr::Attribute::create("path"));
     * @endcode
     */
    Query query_file_path() const;

  private:
    shared_ptr<Database> db_;
};

}

#endif // BRFC_FILE_CATALOGER_H
