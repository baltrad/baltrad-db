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

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

namespace brfc {

class String;

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
     */
    shared_ptr<FileEntry> store(const oh5::PhysicalFile& file);

    /**
     * @brief execute a query
     */
    shared_ptr<FileResult> execute(const FileQuery& query) {
        return do_execute(query);
    }
    
    /**
     * @brief execute a query
     */
    shared_ptr<AttributeResult> execute(const AttributeQuery& query) {
        return do_execute(query);
    }

  protected:
    virtual bool do_is_stored(const oh5::PhysicalFile& file) = 0;
    virtual bool do_remove(const FileEntry& entry) = 0;
    virtual shared_ptr<FileEntry> do_store(const oh5::PhysicalFile& file) = 0;

    virtual shared_ptr<FileResult> do_execute(const FileQuery& query) = 0;
    virtual shared_ptr<AttributeResult> do_execute(const AttributeQuery& query) = 0;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_DATABASE_H
