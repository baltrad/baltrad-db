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

#ifndef BRFC_SQL_COLUMN_HPP
#define BRFC_SQL_COLUMN_HPP

#include <brfc/String.hpp>

#include <brfc/sql/Expression.hpp>

#include <brfc/sql/fwd.hpp>

namespace brfc {
namespace sql {

class Column : public Expression {
  public:
    static ColumnPtr create(SelectablePtr selectable,
                            const String& name) {
        return ColumnPtr(new Column(selectable, name));
    }

    void selectable(SelectablePtr selectable) {
        selectable_ = selectable;
    }

    SelectablePtr selectable() const { return selectable_; }

    void name(const String& name) {
        name_ = name;
    }

    const String& name() const { return name_; }

  protected:
    Column(SelectablePtr selectable,
           const String& name)
            : selectable_(selectable)
            , name_(name) {

    }

  private:
    SelectablePtr selectable_;
    String name_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_COLUMN_HPP
