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

#ifndef BRFC_DB_FILE_QUERY_HPP
#define BRFC_DB_FILE_QUERY_HPP

#include <brfc/expr/fwd.hpp>

#include <vector>

namespace brfc {
namespace db {

class Database;
class FileResult;

/**
 * @brief query against the database
 */
class FileQuery {
  public:
    enum SortDir {
        ASC = 1,
        DESC = 2
    };

    typedef std::pair<expr::ExpressionPtr, SortDir> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief constructor
     * @param db Database instance this FileQuery executes on
     */
    FileQuery(Database* db=0);

    /**
     * @brief copy constructor
     */
    FileQuery(const FileQuery& other);
    
    /**
     * @brief destructor
     */
    ~FileQuery();
    
    /**
     * @brief copy assignment
     */
    FileQuery& operator=(const FileQuery& rhs);
    
    /**
     * @brief access bound database
     */
    db::Database* database() const { return db_; }

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this FileQuery (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    FileQuery& filter(const expr::Expression& expr);
    
    expr::ExpressionPtr filter() const {
        return filter_;
    }
    
    /**
     * @brief append sort order
     */
    FileQuery& order_by(const expr::Expression& expr, SortDir dir);

    OrderVector order() const { return order_; }

    /**
     * @brief limit query results
     */
    FileQuery& limit(int limit) { limit_ = limit; return *this; }

    int limit() const { return limit_; }

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     * @note caller takes ownership of the result
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    FileResult* execute();

  private:
    Database* db_;
    expr::ExpressionPtr filter_;
    OrderVector order_;
    int limit_;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_FILE_QUERY_HPP
