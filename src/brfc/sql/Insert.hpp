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

#include <vector>
#include <utility>

#include <string>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Element.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL INSERT statement
 */
class Insert : public Element {
  public:
    typedef std::vector<std::pair<ColumnPtr, ExpressionPtr> > ValueMap;
    typedef std::vector<ExpressionPtr> ReturnVector;
    
    /**
     * @brief construct as shared_ptr
     * @sa Insert()
     */
    static InsertPtr create(TablePtr table) {
        return InsertPtr(new Insert(table));
    }
    
    /**
     * @brief table this statement inserts to
     */
    TablePtr table() const { return table_; }

    /**
     * @brief bind value to a column
     * @param column name of the column
     * @param expr SQL expression to insert to the column
     * @throw lookup_error if table doesn't have the column
     * @throw duplicate_entry if a value is already bound for the column
     */
    void value(const std::string& column, ExpressionPtr expr);
    
    /**
     * @brief bind value to a column
     * @param column the column
     * @param expr SQL expression to insert to the column
     * @throw lookup_error if table doesn't have the column
     * @throw duplicate_entry if a value is already bound for the column
     */
    void value(ColumnPtr column, ExpressionPtr expr);

    /**
     * @brief access values bound to columns
     */
    const ValueMap& values() const { return values_; }
    
    /**
     * @brief add an expression to return
     */
    void add_return(ExpressionPtr expr);
    
    /**
     * @brief access expressions this statement returns
     */
    const ReturnVector& returns() const { return returns_; }
  
  private:
    /**
     * @brief constructor
     * @param table the table to insert values to
     */
    explicit Insert(TablePtr table)
            : Element()
            , table_(table)
            , values_()
            , returns_() {
    }
  
    TablePtr table_;
    ValueMap values_;
    ReturnVector returns_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_INSERT_HPP
