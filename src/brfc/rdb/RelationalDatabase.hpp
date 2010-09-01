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

#ifndef BRFC_RDB_RELATIONAL_DATABASE_HPP
#define BRFC_RDB_RELATIONAL_DATABASE_HPP

#include <map>

#include <brfc/smart_ptr.hpp>
#include <brfc/Database.hpp>


namespace brfc {

class FileHasher;
class String;
class Variant;

namespace oh5 {

class Source;
class SourceCentre;
class SourceRadar;

} // namespace oh5

namespace sql {

class BindMap;
class Connection;

} // namespace sql

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

    sql::Connection& connection() {
        return *conn_;
    }

    void file_hasher(shared_ptr<FileHasher> hasher);
    
    /**
     * @note caller retains hasher ownership
     */
    void file_hasher(FileHasher* hasher);

    FileHasher& file_hasher() { return *file_hasher_; }

    long long db_id(const oh5::File& file);
    
    long long db_id(const oh5::Source& source);

  protected:
    /**
     * @brief check if file hash is unique in database
     */
    virtual bool do_has_file(const oh5::File& file);
    virtual void do_remove_file(const FileEntry& entry);

    virtual shared_ptr<FileEntry> do_save_file(const oh5::File& file);
    
    virtual shared_ptr<oh5::Source> do_load_source(const String& srcstr);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

  private:
    /**
     * @brief map source id's to source instances
     */
    typedef std::map<long long, shared_ptr<oh5::Source> > SourceMap;

    shared_ptr<oh5::SourceRadar>
    load_source_radar(shared_ptr<oh5::SourceRadar> src);

    shared_ptr<oh5::SourceCentre>
    load_source_centre(shared_ptr<oh5::SourceCentre> src,
                       long long id=0);

    /**
     * @brief save file to database
     */
    id_type save(const oh5::File& file,
                 const String& proposed_filename,
                 unsigned int filename_version);

    /**
     * @brief remove file from database
     */
    void remove_file(const String& path);

    shared_ptr<sql::Connection> conn_;
    shared_ptr<AttributeMapper> mapper_;
    shared_ptr<FileHasher> file_hasher_;
    SourceMap sources_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_RELATIONAL_DATABASE_HPP
