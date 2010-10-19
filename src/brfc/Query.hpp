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

#ifndef BRFC_QUERY_HPP
#define BRFC_QUERY_HPP

#include <brfc/expr/fwd.hpp>

#include <vector>

namespace brfc {

class Database;
class ResultSet;

/**
 * @brief query against the database
 */
class Query {
  public:
    enum SortDirection {
        ASCENDING = 1,
        DESCENDING = 2
    };

    typedef std::vector<expr::ExpressionPtr> ExpressionVector;
    typedef std::pair<expr::ExpressionPtr, SortDirection> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief constructor
     * @param db Database instance this Query executes on
     */
    Query(Database* db);

    /**
     * @brief copy constructor
     */
    Query(const Query& other);
    
    /**
     * @brief destructor
     */
    ~Query();

    /**
     * @brief fetch unique results
     */
    Query& distinct(bool distinct);

    bool distinct() const { return distinct_; }

    /**
     * @brief mark an attribute for fetching
     * @param attribute Attribute to fetch to ResultSet
     * @return this Query (for chaining)
     *
     * attributes are returned in ResultSet in the same order as they
     * are marked.
     */
    Query& fetch(expr::AttributePtr attribute);
    
    /**
     * @brief mark a result for a function for fetching
     * @param function Function whose result to fetch to ResultSet
     * @return this Query (for chaining)
     */
    Query& fetch(expr::FunctionPtr function);

    const ExpressionVector& fetch() const {
        return fetch_;
    }

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this Query (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    Query& filter(expr::ExpressionPtr expr);
    
    expr::ExpressionPtr filter() const {
        return filter_;
    }

    /**
     * @brief append sort order
     */
    Query& order_by(expr::ExpressionPtr expr, SortDirection dir);

    OrderVector order() const { return order_; }
    
    /**
     * @brief limit query results
     */
    Query& limit(int limit) { limit_ = limit; return *this; }

    int limit() const { return limit_; }

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    shared_ptr<ResultSet> execute();
    
  private:
    Database* db_;
    bool distinct_;
    ExpressionVector fetch_;
    expr::ExpressionPtr filter_;
    OrderVector order_;
    int limit_;
};

} // namespace brfc

#endif // BRFC_QUERY_HPP
