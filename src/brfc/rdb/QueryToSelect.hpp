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

#ifndef BRFC_RDB_QUERY_TO_SELECT_HPP
#define BRFC_RDD_QUERY_TO_SELECT_HPP

#include <vector>

#include <brfc/visit.hpp>

#include <brfc/expr/fwd.hpp>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {

class Query;

namespace rdb {

class AttributeMapper;

/**
 * @brief transform a Query to Select statement
 */
class QueryToSelect {
  public:
    typedef mpl::vector<expr::Attribute,
                        expr::BinaryOperator,
                        expr::Label,
                        expr::Literal,
                        expr::Parentheses> accepted_types;
    
    /**
     * @brief transform a Query to Select statement
     *
     * Attributes in Query.fetch() are visited and the results are stored
     * in the "what" clause of the select statement. The Query.filter()
     * expression is also visited and becomes the where clause for the
     * select statement. From clause is formed during the visitation.
     */
    static sql::SelectPtr transform(const Query& query,
                                    const AttributeMapper& mapper);
    
    /**
     * @brief replace expr::Attribute with the mapped Column
     * 
     * Look up the table/column mapping for the attribute and replace the
     * attribute with the column in the expression tree. For non-specialized
     * attributes, the table is aliased based on the attribute name (this
     * enables different attributes to be stored in a common table and still
     * filter them without these filters interfering with eachother).
     *
     * The looked-up table (or table alias) is joined to the from clause.
     */
    void operator()(expr::Attribute& attr);
    
    void operator()(expr::BinaryOperator& op);

//    void operator()(Column& op);

    void operator()(expr::Label& label);

    void operator()(expr::Literal& literal);

    void operator()(expr::Parentheses& parentheses);

 protected: 
    /**
     * @brief constructor
     * @param mapper AttributeMapper instance to fetch mappings from
     *
     * Default from clause is a join from files to sources
     */
    QueryToSelect(const AttributeMapper* mapper);

    sql::ExpressionPtr pop();

    void push(sql::ExpressionPtr p);

    /**
     * @brief join "groups" table if not already joined
     */
    void join_groups();

  private:
    const AttributeMapper* mapper_;
    sql::Factory xpr_;
    std::vector<sql::ExpressionPtr> stack_;
    sql::TablePtr files_t_, src_t_, src_radars_t_, src_centres_t_, groups_t_;
    sql::JoinPtr from_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_QUERY_TO_SELECT_HPP
