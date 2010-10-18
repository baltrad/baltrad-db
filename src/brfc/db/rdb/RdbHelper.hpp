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

#ifndef BRFC_DB_RDB_RDB_HELPER_HPP
#define BRFC_DB_RDB_RDB_HELPER_HPP

#include <brfc/sql/Factory.hpp>

namespace brfc {

     class FileHasher;

namespace oh5 {
    class Attribute;
    class Group;
    class Node;
    class PhysicalFile;
    class Source;
}

namespace sql {
    class Connection;
    class Dialect;
    class Result;
}

namespace db {
namespace rdb {

class Model;
class RdbFileEntry;
class RdbNodeBackend;

/**
 * @brief gather & simplify database queries
 */
class RdbHelper {
  public:
    RdbHelper(sql::Connection* conn, const FileHasher* hasher);

    virtual ~RdbHelper();

    sql::Connection& conn();

    const sql::Dialect& dialect();
    
    /**
     * @brief access RdbNodeBacked on @c node
     * @param node the node to get the backend from
     * @throw runtime_error if no backend or backend is not RdbNodeBackend
     */
    RdbNodeBackend& backend(oh5::Node& node) const;

    /**
     * @brief access RdbNodeBacked on @c node
     * @param node the node to get the backend from
     * @throw runtime_error if no backend or backend is not RdbNodeBackend
     */
    const RdbNodeBackend& backend(const oh5::Node& node) const;

    /**
     * @brief insert @c node to database
     * @param node the node to insert
     * @pre node is associated with RdbFileEntry (and it has database id)
     * @post node backend has database id
     *
     * if @c node is oh5::Attribute, call insert_attribute
     */
    virtual void insert_node(oh5::Node& node);
    
    /**
     * @brief insert @c attr to database
     * @param attr the attribute to insert
     * @pre node backend has database id
     *
     * if attribute value is of type oh5::Scalar::STRING, check if it
     * is convertible to bool, Date and Time and store the value for all
     * successful conversions.
     */
    virtual void insert_attribute(oh5::Attribute& attr);
    
    /**
     * @brief insert @c entry to database
     * @param entry the entry to insert
     * @param file the file this entry is for
     * @post entry has database id
     */
    virtual void insert_file(RdbFileEntry& entry,
                             const oh5::PhysicalFile& file);
    
    /**
     * @brief insert file content to database
     * @param entry the entry the data is for
     * @param path path to the file in filesystem
     *
     * @pre entry has database id
     * @post entry has large object id
     */
    virtual void insert_file_content(RdbFileEntry& entry, const String& path);
    
    /**
     * @brief lookup id for @c source
     * @return database id of the source or 0 if source is not found
     *
     * Model::source_kvs is queryied for key/value pairs defined in source.
     * A source is considered found when exactly one database id is found.
     */
    virtual long long select_source_id(const oh5::Source& source);
    
    /**
     * @brief lookup id for the root node of @c entry
     * @return database id of the node or 0 if not found
     * @pre entry has database id
     */
    virtual long long select_root_id(const RdbFileEntry& entry);
    
    /**
     * @brief load source by database id
     * @return source with all key/value pairs defined in database or
     *         empty source if not found
     */
    virtual oh5::Source select_source(long long id);
    
    /**
     * @brief load @c entry from database
     * @pre entry has database id
     */
    virtual void load_file(RdbFileEntry& entry);
    
    /**
     * @brief load all child nodes of @c node from database
     * @pre node has database id
     */
    virtual void load_children(oh5::Node& node);

  private:
    long long last_id(sql::Result& result);

    sql::Connection* conn_;
    const FileHasher* hasher_;
    const Model& m_;
    sql::Factory sql_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RDB_HELPER_HPP
