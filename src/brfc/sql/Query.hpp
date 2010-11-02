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

#include <brfc/sql/BindMap.hpp>

namespace brfc {
namespace sql {

class Dialect;

class Query {
  public:
    explicit Query(const String& statement="",
                   const BindMap& binds=BindMap())
            : statement_(statement)
            , binds_(binds) {
    }

    Query(const Query& other)
            : statement_(other.statement_)
            , binds_(other.binds_) {
    }

    Query& operator=(const Query& rhs) {
        if (this != &rhs) {
            statement_ = rhs.statement_;
            binds_ = rhs.binds_;
        }
        return *this;
    }
    
    /**
     * @brief bind a value
     * @throw lookup_error if no such bind is defined
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

    void statement(const String& statement) {
        statement_ = statement;
    }

    const String& statement() const {
        return statement_;
    }

    /**
     * @brief replace bind placeholders in the SQL statement
     * @param dialect database dialect the replacement is done for
     * @throw value_error if not all binds consumed or available
     */
    String replace_binds(const Dialect& dialect) const;
    
  private:
    String statement_;
    BindMap binds_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_QUERY_HPP
