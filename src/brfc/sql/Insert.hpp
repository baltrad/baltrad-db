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

#ifndef BRFC_SQL_INSERT_HPP
#define BRFC_SQL_INSERT_HPP

#include <map>
#include <string>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL INSERT statement
 */
class Insert {
  public:
    typedef std::map<std::string, expr::Expression> ValueMap;

    /**
     * @brief constructor
     * @param table the table to insert values to
     */
    explicit Insert(const std::string& table);

    /**
     * @brief table this statement inserts to
     */
    std::string table() const { return table_; }

    /**
     * @brief bind value to a column
     * @param column name of the column
     * @param x SQL expression to insert to the column
     */
    void value(const std::string& column, const expr::Expression& x);
    
    /**
     * @brief add an expression to return
     */
    void returning(const expr::Expression& x);
    
    /**
     * @brief access expressions this statement returns
     */
    expr::Expression returning() const { return returning_; }

    expr::Expression expression() const;
  
  private:  
    std::string table_;
    ValueMap values_;
    expr::Expression returning_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_INSERT_HPP
