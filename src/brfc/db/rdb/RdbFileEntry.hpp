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

#ifndef BRFC_DB_RDB_RDB_FILE_ENTRY_HPP
#define BRFC_DB_RDB_RDB_FILE_ENTRY_HPP

#include <brfc/db/FileEntry.hpp>

#include <brfc/db/rdb/RdbOh5NodeBackend.hpp>

#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

class RelationalDatabase;

/**
 * @brief file entry in relational database
 */
class RdbFileEntry : public FileEntry {
  public:
    /**
     * @brief constructor
     * @param rdb RelationalDatabase this entry is from
     */
    explicit RdbFileEntry(RelationalDatabase* rdb);

    /**
     * @brief destructor
     */
    virtual ~RdbFileEntry();

    using FileEntry::uuid;

    std::string uuid(const std::string& uuid) { return uuid_ = uuid; }

    /**
     * @brief set database id
     */
    void id(long long id) { id_ = id; }
    
    /**
     * @brief get database id
     */
    long long id() const { return id_; }
    
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

    using FileEntry::hash;

    void hash(const std::string& hash) { hash_ = hash; }

    using FileEntry::size;

    void size(long long size) { size_ = size; }

    using FileEntry::stored_at;

    void stored_at(const DateTime& dt) { stored_at_ = dt; }

    bool loaded() const { return loaded_; }

    void loaded(bool loaded) { loaded_ = loaded; }

    /**
     * @brief load database entry
     */
    void load() const;
    
    RdbOh5NodeBackend& node_backend() { return nodes_; }
    const RdbOh5NodeBackend& node_backend() const { return nodes_; }

  protected:
    virtual std::string do_uuid() const;

    virtual void do_write_to_file(const std::string& path) const;

    virtual const Oh5Node& do_root() const;
    
    /**
     * @brief source as stored in the database
     */
    virtual Oh5Source do_source() const;

    virtual std::string do_hash() const;

    virtual long long do_size() const;

    virtual DateTime do_stored_at() const;
    
  private:
    RelationalDatabase* rdb_;
    
    bool loaded_;
    long long id_;
    long long lo_id_;
    long long source_id_;
    Oh5Source source_;
    std::string uuid_;
    std::string hash_;
    long long size_;
    DateTime stored_at_;
    RdbOh5NodeBackend nodes_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RDB_FILE_ENTRY_HPP
