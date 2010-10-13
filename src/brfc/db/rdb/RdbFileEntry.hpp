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

#ifndef BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP
#define BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP

#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {

class FileHasher;

namespace db {
namespace rdb {

class AttributeMapper;
class RelationalDatabase;

/**
 * @brief file entry in relational database
 */
class RdbFileEntry : public FileEntry {
  public:
    /**
     * @brief constructor
     * @param rdb RelationalDatabase this entry is from
     * @param id database id of this entry
     *
     * if id is not 0, id for root node is loaded from database
     */
    RdbFileEntry(RelationalDatabase* rdb, long long id=0);

    /**
     * @brief destructor
     */
    virtual ~RdbFileEntry();

    using FileEntry::id;
    
    /**
     * @brief set database id
     */
    void id(long long id) { id_ = id; }
    
    /**
     * @brief set bound large object id (file content)
     */
    void lo_id(long long id) { lo_id_ = id; }

    /**
     * @brief id of the bound large object (file content)
     *
     * if 0, will call load()
     */
    long long lo_id() const;
    
    /**
     * @brief set source id
     */
    void source_id(long long id) { source_id_ = id; }

    /**
     * @brief id of the bound source
     *
     * if 0, will call load()
     */
    long long source_id() const;
    
    RelationalDatabase& rdb() const { return *rdb_; }

  protected:
    virtual long long do_id() const { return id_; }

    virtual void do_write_to_file(const String& path) const;

    virtual const oh5::Group& do_root() const { return root_; }
    
    /**
     * @brief source as stored in the database
     */
    const oh5::Source& do_source() const;
    
    /**
     * @brief load database entry
     */
    void load() const;

  private:
    
    void insert_file(const String& hasher_name,
                     const String& hash,
                     const String& object,
                     const Date& n_date,
                     const Time& n_time);
    
    void insert_file_content(const String& path);

    RelationalDatabase* rdb_;

    long long id_;
    long long lo_id_;
    long long source_id_;
    oh5::Source source_;
    oh5::RootGroup root_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP
