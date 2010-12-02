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

#ifndef BRFC_SQL_QUERY_HPP
#define BRFC_SQL_QUERY_HPP

#include <brfc/String.hpp>
#include <brfc/StringList.hpp>


#include <brfc/sql/BindMap.hpp>

namespace brfc {
namespace sql {

class Dialect;

class Query {
  public:
    /**
     * @brief constructor
     * @param stmt the sql statement string
     * @param binds bind placeholder values
     */
    explicit Query(const String& stmt="", const BindMap& binds=BindMap());
    
    /**
     * @brief copy constructor
     */
    Query(const Query& other);
    
    /**
     * @brief copy assignment
     */
    Query& operator=(const Query& rhs);
    
    /**
     * @brief bind a value
     * @throw lookup_error if no such bind is defined in binds
     */
    void bind(const String& name, const Variant& value) {
        binds_.set(name, value);
    }

    void binds(const BindMap& binds) {
        binds_ = binds;
    }

    const BindMap& binds() const {
        return binds_;
    }

    void statement(const String& statement) { stmt_ = statement; }

    const String& statement() const { return stmt_; }

    /**
     * @brief replace bind placeholders in the SQL statement
     * @param dialect database dialect the replacement is done for
     * @throw lookup_error if not all binds consumed
     */
    String replace_binds(const Dialect& dialect) const;
    
  private:
    String stmt_;
    BindMap binds_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_QUERY_HPP
