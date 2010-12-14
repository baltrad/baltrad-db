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

#ifndef BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP
#define BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP

#include <vector>

#include <brfc/visit.hpp>

#include <brfc/expr/fwd.hpp>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {

namespace db {

class AttributeQuery;
class FileQuery;

namespace rdb {

class AttributeMapper;
class Model;

/**
 * @brief transform a query to sql::Select statement
 */
class QueryToSelect {
  public:
    typedef mpl::vector<const expr::Attribute,
                        const expr::BinaryOperator,
                        const expr::Function,
                        const expr::Label,
                        const expr::Literal,
                        const expr::Parentheses> accepted_types;
    
    /**
     * @brief constructor
     * @param mapper AttributeMapper to look up specialized attributes
     */
    QueryToSelect(const AttributeMapper* mapper);

    /**
     * @brief transform a FileQuery to sql::Select statement
     * @return the created sql::Select statement
     * 
     * Select distinct 'bdb_files.id, bdb_file_content.lo_id', ordering
     * the results by 'bdb_files.id'. FileQuery::filter() is visited and
     * becomes the where clause for the select statement. The from-clause
     * formed during the visitation becomes the from-clause for the
     * statement.
     */
    sql::SelectPtr transform(const FileQuery& query);
    
    /**
     * @brief transfrom an AttributeQuery to sql::Select statement
     * @return the created sql::Select statement
     * 
     * AttributeQuery::fetch() is visited and the resulting columns passed to
     * sql::Select::what(). AttributeQuery::filter() is visited and becomes
     * the where clause for the select statement. distinct/limit/order are
     * transferred from the query to the statement. The from-clause formed
     * during the visitation becomes the from-clause for the statement.
     */
    sql::SelectPtr transform(const AttributeQuery& query);

    /**
     * @brief turn expr::Attribute into sql::Column
     * @post the expression stack contains equivalent sql::Column
     * 
     * depending on the attribute, column lookup is forwarded to:
     * - source_attr_column(), if the attribute name is 'what/source:*'
     * - specialized_attr_column(), if the attribute is specialized in the
     *   mapper
     * - plain_attr_column() for every other attribute
     */
    void operator()(const expr::Attribute& attr);
    
    /**
     * @brief turn expr::BinaryOperator into sql::BinaryOperator
     * @post the expression stack contains equivalent sql::BinaryOperator
     */
    void operator()(const expr::BinaryOperator& op);
    
    /**
     * @brief turn expr::Function into sql::Function
     * @post the expression stack contains equivalent sql::Function
     */
    void operator()(const expr::Function& func);
    
    /**
     * @brief turn expr::Label into sql::Label
     * @post the expression stack contains equivalent sql::Label
     */
    void operator()(const expr::Label& label);
    
    /**
     * @brief turn expr::Literal into sql::Literal
     * @post the expression stack contains equivalent sql::Literal
     */
    void operator()(const expr::Literal& literal);
    
    /**
     * @brief turn expr::Parentheses into sql::Parentheses
     * @post the expression stack contains equivalent sql::Parentheses
     */
    void operator()(const expr::Parentheses& parentheses);

 protected: 
    /**
     * @brief reset to default state
     *
     * set from clause to 'bdb_files' joined to 'bdb_sources', clear the
     * expression stack
     */
    void reset();
    
    /**
     * @brief pop an expression from the stack
     */
    sql::ExpressionPtr pop();
    
    /**
     * @brief push an expression into the stack
     */
    void push(sql::ExpressionPtr p);

    /**
     * @brief look up sql::Column for a source attribute
     * @return the sql::Column where the attribute is stored
     *
     * The column is looked up using the $KEY in 'what/source:$KEY':
     * - if $KEY is 'name' or 'node', the column for the attribute is
     *   'bdb_sources.name'
     * - for every other $KEY, 'bdb_source_kvs' is joined in the from-clause
     *   (if not already joined) using an alias 'src_$KEY' with an additional
     *   join-condition 'bdb_source_kvs.key=$KEY'. The column for the
     *   attribute is 'src_$KEY.value'.
     */
    sql::ColumnPtr source_attr_column(const expr::Attribute& attr);

    /**
     * @brief look up sql::Column for a specialized attribute
     * @return the sql::Column where the attribute is stored
     *
     * look up mapping from the mapper and use Mapping::column as the column
     * for the attribute.
     */
    sql::ColumnPtr specialized_attr_column(const expr::Attribute& attr);

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
    sql::ColumnPtr plain_attr_column(const expr::Attribute& attr);

  private:
    const AttributeMapper* mapper_;
    sql::Factory xpr_;
    std::vector<sql::ExpressionPtr> stack_; ///< stack of expression pointers
    const Model& m_;
    sql::SelectPtr select_; ///< the select statement
    sql::JoinPtr from_; ///< from clause for the statement
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP
