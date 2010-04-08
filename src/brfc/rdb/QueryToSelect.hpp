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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_RDB_QUERY_TO_SELECT_HPP
#define BRFC_RDD_QUERY_TO_SELECT_HPP

#include <vector>

#include <brfc/visit.hpp>
#include <brfc/expr/fwd.hpp>
#include <brfc/rdb/fwd.hpp>

namespace brfc {

class Query;

namespace rdb {

class AttributeMapper;

/**
 * @brief transform a Query to Select statement
 *
 * The strategy is to first replace all the attributes with columns,
 * collecting the unique tables/aliases. Since the Select has no from
 * clause, we determine a central table and join the remaining tables to it
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
     */
    static SelectPtr transform(const Query& query,
                               const AttributeMapper& mapper);

    /**
     * @brief visit an expr::Attribute element
     *
     * replace this attribute with a column
     * if it's not a specialized column,
     * add a where clause to attribute names
     */
    void operator()(expr::Attribute& attr);

    void operator()(expr::BinaryOperator& op);

    void operator()(expr::Label& label);

    void operator()(expr::Literal& literal);

    void operator()(expr::Parentheses& parentheses);

 protected: 
    /**
     * @brief constructor
     * @param mapper AttributeMapper instance to fetch mappings from
     */
    QueryToSelect(SelectPtr select, const AttributeMapper* mapper);

    void replace_attributes();

    void build_from_clause();

    expr::ElementPtr pop();

    void push(expr::ElementPtr p);

    void join_groups();

  private:
    const AttributeMapper* mapper_;
    std::vector<expr::ElementPtr> stack_;
    SelectPtr select_;
    JoinPtr from_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_QUERY_TO_SELECT_HPP
