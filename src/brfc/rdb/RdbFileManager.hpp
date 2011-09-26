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
#ifndef BRFC_RDB_FILE_MANAGER_HPP
#define BRFC_RDB_FILE_MANAGER_HPP

#include <boost/noncopyable.hpp>

namespace brfc {
    class Oh5Node;
    class RdbFileEntry;
}

namespace brfc {

class RdbFileManager : public boost::noncopyable {
  public:
    virtual ~RdbFileManager();

    /**
     * @brief insert @c entry to database
     * @param entry the entry to insert
     * @return database id of the entry
     */
    long long insert_file(const RdbFileEntry& entry) {
        return do_insert_file(entry);
    }
    
    /**
     * @brief insert file content to database
     * @param file_id the entry id the data is for
     * @param path path to the file in filesystem
     * @return large object id of the file content
     */
    long long insert_file_content(long long file_id,
                                  const std::string& path) {
        return do_insert_file_content(file_id, path);
    }

    /**
     * @brief insert nodes into the database
     * @param file_id id if the file to associate the nodes with
     * @param root the root node
     */
    void insert_nodes(long long file_id, const Oh5Node& root) {
        do_insert_nodes(file_id, root);
    }

    /**
     * @brief query uuid for source_id and hash combination
     * @return the uuid or empty string if not found
     */
    std::string uuid_by_source_and_hash(long long source_id,
                                        const std::string& hash) {
        return do_uuid_by_source_and_hash(source_id, hash);
    }
    
    /**
     * @brief load @c entry from database
     * @pre entry has database id
     */
    void load_file(RdbFileEntry& entry) {
        do_load_file(entry);
    }
    
    /**
     * @brief load nodes associated with file from database
     * @param file_id database id of the file whose nodes to load
     * @param root root node loaded nodes will be attached to
     */
    void load_nodes(long long file_id, Oh5Node& root) {
        do_load_nodes(file_id, root);
    }
    
    /**
     * @brief remove file entry from the database
     */
    bool remove_file(const std::string& uuid) {
        return do_remove_file(uuid);
    }

  private:
    virtual long long do_insert_file(const RdbFileEntry& entry) = 0;
    virtual long long do_insert_file_content(long long file_id,
                                             const std::string& path) = 0;
    virtual void do_insert_nodes(long long file_id, const Oh5Node& root) = 0;
    virtual std::string do_uuid_by_source_and_hash(long long source_id,
                                                   const std::string& hash) = 0;
    virtual void do_load_file(RdbFileEntry& entry) = 0;
    virtual void do_load_nodes(long long file_id, Oh5Node& root) = 0;
    virtual bool do_remove_file(const std::string& uuid) = 0;
};

inline
RdbFileManager::~RdbFileManager() { }

} // namespace brfc

#endif // BRFC_RDB_FILE_MANAGER_HPP
