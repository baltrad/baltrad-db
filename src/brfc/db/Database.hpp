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

#ifndef BRFC_DB_DATABASE_H
#define BRFC_DB_DATABASE_H

#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/oh5/Source.hpp>

namespace brfc {

namespace oh5 {

class PhysicalFile;

} // namespace oh5

namespace db {

class AttributeQuery;
class AttributeResult;
class FileEntry;
class FileQuery;
class FileResult;

/**
 * @brief ABC for Database access
 */
class Database : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~Database() { }
    
    /**
     * @brief is file stored
     */
    bool is_stored(const oh5::PhysicalFile& file) {
        return do_is_stored(file);
    }
    
    /**
     * @brief remove file from database
     * @throw db_error if a database error occurs
     * @return true if the file was removed, false if it did not exist
     */
    bool remove(const FileEntry& entry) {
        return do_remove(entry);
    }

    /**
     * @brief save file to database
     * @param file the file to be saved
     * @return a FileEntry instance for the saved file
     * @throw db_error if a database error occurs
     * @throw duplicate_entry if file is already stored to database
     * @note caller takes ownership of the entry
     */
    FileEntry* store(const oh5::PhysicalFile& file) {
        return do_store(file);
    }
    
    /**
     * @brief get a FileEntry for @c file, storing it if not already stored
     * @throw db_error if a database error occurs
     * @note caller takes ownership of the entry
     *
     * this is equivalent to:
     * @code
     * if (is_stored(file)) {
     *    return entry_by_file(file);
     * } else {
     *    return store(file);
     * }
     * @endcode
     */
    FileEntry* get_or_store(const oh5::PhysicalFile& file);
    
    /**
     * @brief get FileEntry by physical file
     * @param file the physical file to look up
     * @note caller takes ownership of the entry
     */
    FileEntry* entry_by_file(const oh5::PhysicalFile& file) {
        return do_entry_by_file(file);
    }

    /**
     * @brief get FileEntry by UUID
     * @param uuid the UUID of requested entry
     * @note caller takes ownership of the entry
     */
    FileEntry* entry_by_uuid(const std::string& uuid) {
        return do_entry_by_uuid(uuid);
    }

    /**
     * @brief execute a query
     */
    FileResult* execute(const FileQuery& query) {
        return do_execute(query);
    }
    
    /**
     * @brief execute a query
     */
    AttributeResult* execute(const AttributeQuery& query) {
        return do_execute(query);
    }
    
    /**
     * @brief list sources defined in the database
     *
     * @note in Java, this method is renamed to _sources(), returning a
     *       eu.baltrad.fc.oh5.SourceVector. There is a wrapper method
     *       sources() returning a List<eu.baltrad.fc.oh5.Source>.
     */
    std::vector<oh5::Source> sources() const {
        return do_sources();
    }
    
    /**
     * @brief add a source definition to this database
     */
    void add_source(const oh5::Source& source) {
        do_add_source(source);
    }

    /**
     * @brief update a source definition in this database
     */
    void update_source(const oh5::Source& source) {
        do_update_source(source);
    }
    
    /**
     * @brief remove a source definition from this database
     */
    void remove_source(const oh5::Source& source) {
        do_remove_source(source);
    }
   
  protected:
    virtual bool do_is_stored(const oh5::PhysicalFile& file) = 0;
    virtual bool do_remove(const FileEntry& entry) = 0;
    virtual FileEntry* do_store(const oh5::PhysicalFile& file) = 0;
    virtual FileEntry* do_entry_by_file(const oh5::PhysicalFile& file) = 0;
    virtual FileEntry* do_entry_by_uuid(const std::string& uuid) = 0;

    virtual FileResult* do_execute(const FileQuery& query) = 0;
    virtual AttributeResult* do_execute(const AttributeQuery& query) = 0;

    virtual std::vector<oh5::Source> do_sources() const = 0;
    virtual void do_add_source(const oh5::Source& source) = 0;
    virtual void do_update_source(const oh5::Source& source) = 0;
    virtual void do_remove_source(const oh5::Source& source) = 0;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_DATABASE_H
