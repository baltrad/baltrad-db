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

#ifndef BRFC_DB_RDB_RDB_HELPER_HPP
#define BRFC_DB_RDB_RDB_HELPER_HPP

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <brfc/expr/Expression.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {

    class FileHasher;
    class Oh5Attribute;
    class Oh5Group;
    class Oh5Node;
    class Oh5PhysicalFile;
    class Oh5Source;
    class RdbFileEntry;
    class RdbOh5NodeBackend;

    namespace sql {
        class Connection;
        class Dialect;
        class Result;
    }
}

namespace brfc {

/**
 * @brief gather & simplify database queries
 */
class RdbHelper : boost::noncopyable {
  public:
    RdbHelper(boost::shared_ptr<sql::Connection> conn);

    virtual ~RdbHelper();

    sql::Connection& conn();

    const sql::Dialect& dialect();
    
    /**
     * @brief insert nodes into the database
     * @param file_id id if the file to associate the nodes with
     * @param root the root node
     */
    void insert_nodes(long long file_id, const Oh5Node& root);

    /**
     * @brief insert @c node to database
     * @param file_id database id of the file entry
     * @param parent_id database id of the parent node
     * @param node the node to insert
     * @return database id of the inserted node
     *
     * if @c node is Attribute, call insert_attribute
     */
    long long insert_node(long long file_id,
                          long long parent_id,
                          const Oh5Node& node);
    
    /**
     * @brief insert @c attr to database
     * @param node_id database id of the node entry
     * @param attr the attribute to insert
     *
     * if attribute value is of type Scalar::STRING, check if it
     * is convertible to bool, Date and Time and store the value for all
     * successful conversions.
     */
    void insert_attribute(long long node_id, const Oh5Attribute& attr);

    /**
     * @brief insert @c entry to database
     * @param entry the entry to insert
     * @return database id of the entry
     */
    long long insert_file(const RdbFileEntry& entry);
    
    /**
     * @brief insert file content to database
     * @param entry_id the entry id the data is for
     * @param path path to the file in filesystem
     * @param size the file size
     * @return large object id of the file content
     */
    long long insert_file_content(long long entry_id,
                                  const std::string& path,
                                  long long size);
    
    /**
     * @brief lookup id for @c source
     * @return database id of the source or 0 if source is not found
     *
     * m::source_kvs is queryied for key/value pairs defined in source.
     * A source is considered found when exactly one database id is found.
     */
    long long select_source_id(const Oh5Source& source);
        
    /**
     * @brief load source by database id
     * @return source with all key/value pairs defined in database or
     *         empty source if not found
     */
    Oh5Source select_source(long long id);
    
    /**
     * @brief load @c entry from database
     * @pre entry has database id
     */
    void load_file(RdbFileEntry& entry);
    
    /**
     * @brief load nodes associated with file from database
     * @param file_id database id of the file whose nodes to load
     * @param root root node loaded nodes will be attached to
     */
    void load_nodes(long long file_id, Oh5Node& root);

    std::vector<Oh5Source> select_all_sources();

    void add_source(const Oh5Source& source);

    void update_source(const Oh5Source& source);

    void remove_source(const Oh5Source& source);

  private:
    long long last_id(sql::Result& result);

    void compile_insert_node_query();
    void compile_insert_attr_query();

    boost::shared_ptr<sql::Connection> conn_;
    sql::Factory sql_;
    Expression insert_node_qry_;
    Expression insert_attr_qry_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RDB_HELPER_HPP
