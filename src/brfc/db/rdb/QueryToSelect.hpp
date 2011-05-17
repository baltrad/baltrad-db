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

#ifndef BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP
#define BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP

#include <boost/scoped_ptr.hpp>

namespace brfc {

    class AttributeMapper;
    class AttributeQuery;
    class FileQuery;

    namespace sql {
        class Select;
    }
}

namespace brfc {

/**
 * @brief transform a query to sql::Select statement
 */
class QueryToSelect {
  public:
    /**
     * @brief constructor
     * @param mapper AttributeMapper to look up specialized attributes
     */
    QueryToSelect(const AttributeMapper* mapper);

    ~QueryToSelect();

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
    sql::Select transform(const FileQuery& query);
    
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
    sql::Select transform(const AttributeQuery& query);

  private:
    struct Impl;
    boost::scoped_ptr<Impl> impl_;

};

} // namespace brfc

#endif // BRFC_DB_RDB_FILE_QUERY_TO_SELECT_HPP
