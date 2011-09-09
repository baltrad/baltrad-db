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
#ifndef BRFC_RDB_FILE_STORAGE_POLICY_HPP
#define BRFC_RDB_FILE_STORAGE_POLICY_HPP

#include <string>

namespace brfc {

class RdbFileEntry;
class RelationalDatabase;

class RdbFileStoragePolicy {
  public:
    virtual ~RdbFileStoragePolicy() = 0;
    
    /**
     * @brief associate with a relational database
     */
    void database(RelationalDatabase* db) {
        do_database(db);
    }

    /**
     * @note store also has to deal with putting metadata to tables
     *       because some implementers (might) want to share the
     *       transaction.
     */
    void store(RdbFileEntry& e, const std::string& path) {
        return do_store(e, path);
    }
    
    /**
     * @brief remove the file from the database and storage
     */
    bool remove(const RdbFileEntry& e) {
        return do_remove(e);
    }
    
    /**
     * @brief write the fileentry to a physical file at @c path
     */
    void write_to_file(const RdbFileEntry& e, const std::string& path) {
        do_write_to_file(e, path);
    }
  
  private:
    virtual void do_database(RelationalDatabase* db) = 0;

    virtual void do_store(RdbFileEntry& e, const std::string& path) = 0;

    virtual bool do_remove(const RdbFileEntry& e) = 0;

    virtual void do_write_to_file(const RdbFileEntry& e,
                                  const std::string& path) = 0;
};

inline
RdbFileStoragePolicy::~RdbFileStoragePolicy() { }

} // namespace brfc

#endif // BRFC_RDB_FILE_STORAGE_POLICY_HPP
