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
#ifndef BRFC_RDB_DEFAULT_FILE_MANAGER_HPP
#define BRFC_RDB_DEFAULT_FILE_MANAGER_HPP

#include <boost/shared_ptr.hpp>

#include <brfc/expr/Expression.hpp>
#include <brfc/rdb/RdbFileManager.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {

    class Oh5Attribute;
    class Oh5Group;

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
class RdbDefaultFileManager : public RdbFileManager {
  public:
    RdbDefaultFileManager(boost::shared_ptr<sql::Connection> conn);

    virtual ~RdbDefaultFileManager();

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


  private:
    virtual long long do_insert_file(const RdbFileEntry& entry);
    virtual long long do_insert_file_content(long long file_id,
                                             const std::string& path);
    virtual void do_insert_nodes(long long file_id, const Oh5Node& root);
    virtual std::string do_uuid_by_source_and_hash(long long source_id,
                                                   const std::string& hash);
    virtual void do_load_file(RdbFileEntry& entry);
    virtual void do_load_nodes(long long file_id, Oh5Node& root);
    virtual bool do_remove_file(const std::string& uuid);


    sql::Connection& conn();

    const sql::Dialect& dialect();

    long long last_id(sql::Result& result);

    void compile_insert_node_query();
    void compile_insert_attr_query();

    boost::shared_ptr<sql::Connection> conn_;
    sql::Factory sql_;
    Expression insert_node_qry_;
    Expression insert_attr_qry_;
};

} // namespace brfc

#endif // BRFC_RDB_QUERY_HPP
