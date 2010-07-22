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
    static ColumnPtr create(const String& name,
                            SelectablePtr selectable=SelectablePtr()) {
        return ColumnPtr(new Column(name, selectable));
    }

    void selectable(SelectablePtr selectable) {
        selectable_ = selectable;
    }
    
    ColumnPtr rebase(SelectablePtr t) const;

    SelectablePtr selectable() const { return selectable_; }

    void name(const String& name) {
        name_ = name;
    }

    virtual const String& name() const { return name_; }

  protected:
    Column(const String& name,
           SelectablePtr selectable)
            : name_(name)
            , selectable_(selectable) {

    }

  private:
    String name_;
    SelectablePtr selectable_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_COLUMN_HPP
