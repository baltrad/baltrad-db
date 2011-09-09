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

#ifndef BRFC_DB_RDB_RELATIONAL_DATABASE_HPP
#define BRFC_DB_RDB_RELATIONAL_DATABASE_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <brfc/db/Database.hpp>

namespace brfc {

    class AttributeMapper;
    class FileHasher;
    class Oh5File;
    class Oh5Source;
    class RdbFileEntry;
    class RdbFileStoragePolicy;
    class RdbHelper;
    class Url;
    class Variant;

    namespace sql {
        class Connection;
        class ConnectionCreator;
        class ConnectionPool;
    }
}

namespace brfc {

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
    explicit RelationalDatabase(const Url& dsn);
    
    explicit RelationalDatabase(boost::shared_ptr<sql::ConnectionPool> pool);

    /**
     * @brief destructor
     */
    virtual ~RelationalDatabase();

    AttributeMapper& mapper();
    
    const AttributeMapper& mapper() const;
    
    /**
     * @brief set storage policy
     * @param policy storage policy to set (caller retains ownership)
     */
    void storage_policy(RdbFileStoragePolicy* policy);
    
    /**
     * @brief acquire a connection from the associated pool
     */
    boost::shared_ptr<sql::Connection> conn() const;    

    FileHasher& file_hasher() { return *file_hasher_; }

    static sql::ConnectionPool* create_pool(sql::ConnectionCreator* conn_ctor,
                                            const Url& dsn);

    void entry_to_file(const RdbFileEntry& entry, const std::string& path);
    RdbFileEntry* file_to_entry(const Oh5PhysicalFile& file);

  protected:
    /**
     * @brief check if file hash is unique in database
     */
    virtual bool do_is_stored(const Oh5PhysicalFile& file);
    virtual bool do_remove(const FileEntry& entry);

    virtual FileEntry* do_store(const Oh5PhysicalFile& file);
    virtual FileEntry* do_entry_by_file(const Oh5PhysicalFile& file);
    virtual FileEntry* do_entry_by_uuid(const std::string& uuid);
    
    virtual FileResult* do_execute(const FileQuery& query);

    virtual AttributeResult* do_execute(const AttributeQuery& query);

    virtual std::vector<Oh5Source> do_sources() const;
    virtual void do_add_source(const Oh5Source& source);
    virtual void do_update_source(const Oh5Source& source);
    virtual void do_remove_source(const Oh5Source& source);

  private:
    void init();

    void populate_mapper();
    void populate_hasher();

    boost::scoped_ptr<sql::ConnectionCreator> creator_;
    boost::shared_ptr<sql::ConnectionPool> pool_;
    boost::shared_ptr<RdbFileStoragePolicy> storage_;
    boost::shared_ptr<AttributeMapper> mapper_;
    boost::shared_ptr<FileHasher> file_hasher_;
    boost::shared_ptr<RdbHelper> helper_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RELATIONAL_DATABASE_HPP
