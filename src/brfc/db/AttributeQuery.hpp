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

#ifndef BRFC_DB_ATTRIBUTE_QUERY_HPP
#define BRFC_DB_ATTRIBUTE_QUERY_HPP

#include <map>
#include <string>
#include <vector>

#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace db {

/**
 * @brief query against the database
 */
class AttributeQuery {
  public:
    enum SortDir {
        ASC = 1,
        DESC = 2
    };

    typedef std::map<std::string, expr::sexp> FetchMap;
    typedef std::vector<expr::sexp> ExpressionVector;
    typedef std::pair<expr::sexp, SortDir> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief constructor
     */
    AttributeQuery();

    /**
     * @brief copy constructor
     */
    AttributeQuery(const AttributeQuery& other);
    
    /**
     * @brief destructor
     */
    ~AttributeQuery();

    /**
     * @brief copy assign
     */
    AttributeQuery& operator=(AttributeQuery rhs);

    void swap(AttributeQuery& other);

    /**
     * @brief fetch unique results
     */
    AttributeQuery& distinct(bool distinct);

    bool distinct() const { return distinct_; }

    /**
     * @brief mark an expression for fetching
     * @param name the name this expression will be accessible with in results
     * @param expr expr::sexp to fetch
     * @return this AttributeQuery (for chaining)
     *
     * expression are returned in AttributeResult in the same order as they
     * are marked.
     */
    AttributeQuery& fetch(const std::string& name,
                          const expr::sexp& expr);
    
    const FetchMap& fetch() const {
        return fetch_;
    }

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this AttributeQuery (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    AttributeQuery& filter(const expr::sexp& expr);
    
    const expr::sexp& filter() const {
        return filter_;
    }
    
    /**
     * @brief add a grouping expression
     * @param expr group expression
     * @return this AttributeQuery (for chaining)
     */
    AttributeQuery& group(const expr::sexp& expr);

    ExpressionVector group() const { return group_; }

    /**
     * @brief append sort order
     */
    AttributeQuery& order_by(const expr::sexp& expr, SortDir dir);

    OrderVector order() const { return order_; }
    
    /**
     * @brief limit query results
     */
    AttributeQuery& limit(int limit) { limit_ = limit; return *this; }

    int limit() const { return limit_; }
    
  private:
    bool distinct_;
    FetchMap fetch_;
    expr::sexp filter_;
    ExpressionVector group_;
    OrderVector order_;
    int limit_;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_ATTRIBUTE_QUERY_HPP
