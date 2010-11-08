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

#ifndef BRFC_DB_ATTRIBUTE_QUERY_HPP
#define BRFC_DB_ATTRIBUTE_QUERY_HPP

#include <brfc/expr/fwd.hpp>

#include <vector>

namespace brfc {
namespace db {

class Database;
class AttributeResult;

/**
 * @brief query against the database
 */
class AttributeQuery {
  public:
    enum SortDir {
        ASC = 1,
        DESC = 2
    };

    typedef std::vector<expr::ExpressionPtr> ExpressionVector;
    typedef std::pair<expr::ExpressionPtr, SortDir> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief constructor
     * @param db Database instance this AttributeQuery executes on
     */
    AttributeQuery(Database* db);

    /**
     * @brief copy constructor
     */
    AttributeQuery(const AttributeQuery& other);
    
    /**
     * @brief destructor
     */
    ~AttributeQuery();

    /**
     * @brief fetch unique results
     */
    AttributeQuery& distinct(bool distinct);

    bool distinct() const { return distinct_; }

    /**
     * @brief mark an attribute for fetching
     * @param attribute Attribute to fetch to ResultSet
     * @return this AttributeQuery (for chaining)
     *
     * attributes are returned in ResultSet in the same order as they
     * are marked.
     */
    AttributeQuery& fetch(const expr::Attribute& attribute);
    
    /**
     * @brief mark a result for a function for fetching
     * @param function Function whose result to fetch to ResultSet
     * @return this AttributeQuery (for chaining)
     */
    AttributeQuery& fetch(const expr::Function& function);

    const ExpressionVector& fetch() const {
        return fetch_;
    }

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this AttributeQuery (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    AttributeQuery& filter(const expr::Expression& expr);
    
    const expr::ExpressionPtr filter() const {
        return filter_;
    }

    /**
     * @brief append sort order
     */
    AttributeQuery& order_by(const expr::Expression& expr, SortDir dir);

    OrderVector order() const { return order_; }
    
    /**
     * @brief limit query results
     */
    AttributeQuery& limit(int limit) { limit_ = limit; return *this; }

    int limit() const { return limit_; }

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    shared_ptr<AttributeResult> execute();
    
  private:
    Database* db_;
    bool distinct_;
    ExpressionVector fetch_;
    expr::ExpressionPtr filter_;
    OrderVector order_;
    int limit_;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_ATTRIBUTE_QUERY_HPP
