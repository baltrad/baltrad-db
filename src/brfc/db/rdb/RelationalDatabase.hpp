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

#ifndef BRFC_DB_RDB_RELATIONAL_DATABASE_HPP
#define BRFC_DB_RDB_RELATIONAL_DATABASE_HPP

#include <map>

#include <brfc/smart_ptr.hpp>

#include <brfc/db/Database.hpp>


namespace brfc {

class FileHasher;
class String;
class Variant;

namespace oh5 {

class File;
class Source;

} // namespace oh5

namespace sql {

class BindMap;
class Connection;

} // namespace sql

namespace db {
namespace rdb {

class AttributeMapper;

/**
 * @brief Relational database backend
 */
class RelationalDatabase : public Database {
  public:
    typedef Variant id_type;

    /**
     * @brief constructor
     * @param dsn connection  in URI-like syntax: 
     *            <tt> engine://user:password\@host:port/dbname </tt>
     *
     * the only engine currently supported is 'postgresql'
     */
    explicit RelationalDatabase(const String& dsn);
    
    /**
     * @brief destructor
     */
    virtual ~RelationalDatabase();

    shared_ptr<AttributeMapper> mapper();
    
    shared_ptr<const AttributeMapper> mapper() const;

    void populate_mapper();

    void populate_hasher();

    sql::Connection& connection() {
        return *conn_;
    }
    
    shared_ptr<sql::Connection> connection_ptr() {
        return conn_;
    }

    void file_hasher(shared_ptr<FileHasher> hasher);
    
    /**
     * @note caller retains hasher ownership
     */
    void file_hasher(FileHasher* hasher);

    FileHasher& file_hasher() { return *file_hasher_; }

    long long db_id(const oh5::File& file);
    
    long long db_id(const oh5::Source& source);

    oh5::Source load_source(const String& srcstr);

  protected:
    /**
     * @brief check if file hash is unique in database
     */
    virtual bool do_has_file(const oh5::PhysicalFile& file);
    virtual bool do_remove_file(const FileEntry& entry);

    virtual shared_ptr<FileEntry> do_save_file(const oh5::PhysicalFile& file);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

  private:
    shared_ptr<sql::Connection> conn_;
    shared_ptr<AttributeMapper> mapper_;
    shared_ptr<FileHasher> file_hasher_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RELATIONAL_DATABASE_HPP