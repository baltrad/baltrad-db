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

#ifndef BRFC_DB_FILE_QUERY_HPP
#define BRFC_DB_FILE_QUERY_HPP

#include <vector>

#include <brfc/expr/Expression.hpp>

namespace brfc {

/**
 * @brief query against the database
 * @ingroup exposed
 */
class FileQuery {
  public:
    enum SortDir {
        ASC = 1,
        DESC = 2
    };

    typedef std::pair<Expression, SortDir> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief constructor
     */
    FileQuery();

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
    FileQuery& operator=(FileQuery rhs);

    void swap(FileQuery& other);
    
    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this FileQuery (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    FileQuery& filter(const Expression& expr);
    
    Expression filter() const {
        return filter_;
    }
    
    /**
     * @brief append sort order
     */
    FileQuery& order_by(const Expression& expr, SortDir dir);

    OrderVector order() const { return order_; }

    /**
     * @brief limit query results to @c n entries
     */
    FileQuery& limit(int n) { limit_ = n; return *this; }

    int limit() const { return limit_; }

    /**
     * @brief skip @c n entries in query results
     * @return this FileQuery (for chaining)
     */
    FileQuery& skip(int n) { skip_ = n; return *this; }

    int skip() const { return skip_; }

  private:
    Expression filter_;
    OrderVector order_;
    int limit_;
    int skip_;
};

} // namespace brfc

#endif // BRFC_DB_FILE_QUERY_HPP
