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

#include <brfc/smart_ptr.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {

    class FileHasher;
    class Oh5Attribute;
    class Oh5Group;
    class Oh5Node;
    class PhysicalOh5File;
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
    RdbHelper(shared_ptr<sql::Connection> conn);

    virtual ~RdbHelper();

    sql::Connection& conn();

    const sql::Dialect& dialect();
    
    /**
     * @brief access RdbNodeBacked on @c node
     * @param node the node to get the backend from
     * @throw runtime_error if no backend or backend is not RdbOh5NodeBackend
     */
    RdbOh5NodeBackend& backend(Oh5Node& node) const;

    /**
     * @brief access RdbNodeBacked on @c node
     * @param node the node to get the backend from
     * @throw runtime_error if no backend or backend is not RdbOh5NodeBackend
     */
    const RdbOh5NodeBackend& backend(const Oh5Node& node) const;

    /**
     * @brief insert @c node to database
     * @param node the node to insert
     * @pre node is associated with RdbFileEntry (and it has database id)
     * @post node backend has database id
     *
     * if @c node is Attribute, call insert_attribute
     */
    void insert_node(long long file_id, Oh5Node& node);
    
    /**
     * @brief insert @c attr to database
     * @param attr the attribute to insert
     * @pre node backend has database id
     *
     * if attribute value is of type Scalar::STRING, check if it
     * is convertible to bool, Date and Time and store the value for all
     * successful conversions.
     */
    void insert_attribute(Oh5Attribute& attr);
    
    /**
     * @brief insert @c entry to database
     * @param entry the entry to insert
     * @param file the file this entry is for
     * @post entry has database id
     */
    void insert_file(RdbFileEntry& entry,
                             const PhysicalOh5File& file);
    
    /**
     * @brief insert file content to database
     * @param entry the entry the data is for
     * @param path path to the file in filesystem
     *
     * @pre entry has database id
     * @post entry has large object id
     */
    void insert_file_content(RdbFileEntry& entry, const std::string& path);
    
    /**
     * @brief lookup id for @c source
     * @return database id of the source or 0 if source is not found
     *
     * m::source_kvs is queryied for key/value pairs defined in source.
     * A source is considered found when exactly one database id is found.
     */
    long long select_source_id(const Oh5Source& source);
    
    /**
     * @brief lookup id for the root node of @c entry
     * @return database id of the node or 0 if not found
     * @pre entry has database id
     */
    long long select_root_id(const RdbFileEntry& entry);
    
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
     * @brief load all child nodes of @c node from database
     * @pre node has database id
     */
    void load_children(Oh5Node& node);

    std::vector<Oh5Source> select_all_sources();

    void add_source(const Oh5Source& source);

    void update_source(const Oh5Source& source);

    void remove_source(const Oh5Source& source);

  private:
    long long last_id(sql::Result& result);

    void compile_insert_node_query();
    void compile_insert_attr_query();

    shared_ptr<sql::Connection> conn_;
    sql::Factory sql_;
    Expression insert_node_qry_;
    Expression insert_attr_qry_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RDB_HELPER_HPP