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
    /**
     * @brief constructor
     * @param db Database instance this FileQuery executes on
     */
    FileQuery(Database* db);

    /**
     * @brief copy constructor
     */
    FileQuery(const FileQuery& other);
    
    /**
     * @brief destructor
     */
    ~FileQuery();

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this FileQuery (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    FileQuery& filter(expr::ExpressionPtr expr);
    
    expr::ExpressionPtr filter() const {
        return filter_;
    }

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    shared_ptr<FileResult> execute();

  private:
    Database* db_;
    expr::ExpressionPtr filter_;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_FILE_QUERY_HPP