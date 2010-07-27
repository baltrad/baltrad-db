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

#ifndef BRFC_SQL_TABLE_HPP
#define BRFC_SQL_TABLE_HPP

#include <map>

#include <brfc/String.hpp>

#include <brfc/sql/Selectable.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL Table
 */
class Table : public Selectable {
  public:
    /**
     * @brief create as smart_ptr
     * @sa Table()
     */
    static TablePtr create(const String& name) {
        return TablePtr(new Table(name));
    }
    
    /**
     * @brief name of this table
     */
    virtual String name() const {
        return name_;
    }
    
    /**
     * @throw duplicate_entry if the table already has a column
     *        with the same name
     * @return the created column
     */
    ColumnPtr add_column(const String& column);
    
    virtual ColumnPtr column(const String& name) const;

    virtual std::vector<ColumnPtr> columns() const;

  protected:
    /**
     * @brief constructor
     * @param name the name of this table
     */
    explicit Table(const String& name)
            : name_(name)
            , columns_() {
    }

  private:
    typedef std::map<String, ColumnPtr> ColumnMap;

    String name_;
    ColumnMap columns_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_TABLE_HPP
