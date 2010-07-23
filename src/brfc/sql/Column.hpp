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
    ColumnPtr shared_from_this() const {
        return static_pointer_cast<Column>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }

    static ColumnPtr create(const String& name,
                            SelectablePtr selectable,
                            ColumnPtr parent_=ColumnPtr()) {
        return ColumnPtr(new Column(name, selectable, parent_));
    }


    virtual const String& name() const { return name_; }
    
    virtual SelectablePtr selectable() const { return selectable_; }

    virtual SelectablePtr base_selectable() const {
        if (parent_)
            return parent_->base_selectable();
        return selectable_;
    }

    virtual void references(ColumnPtr column);

    virtual ColumnPtr references() const {
        if (parent_)
            return parent_->references();
        return references_;
    }

    ColumnPtr proxy(SelectablePtr t) const;

    ColumnPtr proxy(const String& name, SelectablePtr t) const;
  
  protected:
    Column(const String& name,
           SelectablePtr selectable,
           ColumnPtr parent)
            : name_(name)
            , selectable_(selectable)
            , parent_(parent) {
    }

  private:
    String name_;
    SelectablePtr selectable_;
    ColumnPtr parent_;
    ColumnPtr references_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_COLUMN_HPP
