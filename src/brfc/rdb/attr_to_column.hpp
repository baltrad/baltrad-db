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
#ifndef BRFC_RDB_ATTR_TO_COLUMN_HPP
#define BRFC_RDB_ATTR_TO_COLUMN_HPP

#include <map>

#include <brfc/sql/Factory.hpp>
#include <brfc/sql/FromClause.hpp>

namespace brfc {

class AttributeMapper;

/**
 * @brief transform (attr "name" "type") to (column "table" "column")
 */
class attr_to_column {
  public:
    typedef Expression result_type;

    attr_to_column(const AttributeMapper* mapper);
    
    Expression operator()(const Expression& x);

   /**
     * @brief turn Attribute into sql::Column
     * @post the expression stack contains equivalent sql::Column
     * 
     * depending on the attribute, column lookup is forwarded to:
     * - source_attr_column(), if the attribute name is 'what/source:*'
     * - specialized_attr_column(), if the attribute is specialized in the
     *   mapper
     * - plain_attr_column() for every other attribute
     */
    Expression column_by_attr(const std::string& name,
                              const std::string& type);

    /**
     * @brief look up sql::Column for a specialized attribute
     * @return the sql::Column where the attribute is stored
     *
     * look up mapping from the mapper and use Mapping::column as the column
     * for the attribute.
     */
    Expression specialized_attr_column(const std::string& name);

    /**
     * @brief look up sql::Column for a source attribute
     * @return the sql::Column where the attribute is stored
     *
     * The column is looked up using the $KEY in 'what/source:$KEY':
     * - if $KEY is '_name', the column for the attribute is 'bdb_sources.name'
     * - for every other $KEY, 'bdb_source_kvs' is joined in the from-clause
     *   (if not already joined) using an alias 'src_$KEY' with an additional
     *   join-condition 'bdb_source_kvs.key=$KEY'. The column for the
     *   attribute is 'src_$KEY.value'.
     */
    Expression source_attr_column(const std::string& name);

    /**
     * @brief look up sql::Column for a plain attribute
     * @return the sql::Column where the attribute is stored
     *
     * Call join_attrs() to join 'attrs' and 'pnode1'. Alias
     * 'bdb_attribute_values' for this attribute as '$NAME_values' and join
     * it in the from clause (if not already joined).
     *
     * The column for the attribute is '$NAME_values.value_$TYPE'.
     */
    Expression plain_attr_column(std::string name,
                                 const std::string& type);
 
    /**
     * @brief reset the state
     */
    void reset();
    
    /**
     * @brief access the from clause
     */
    sql::FromClause from() const { return from_; }

  private:
    void join_sources();

    std::string join_source_kvs(const std::string& key);

    const AttributeMapper* mapper_;
    sql::Factory xpr_;
    sql::FromClause from_; ///< from clause for the statement
    std::map<std::string, std::string> plain_column_map;
};

} // namespace brfc

#endif // BRFC_RDB_ATTR_TO_COLUMN_HPP
