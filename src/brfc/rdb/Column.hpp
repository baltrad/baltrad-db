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

#ifndef BRFC_RDB_COLUMN_HPP
#define BRFC_RDB_COLUMN_HPP

#include <brfc/rdb/fwd.hpp>
#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace rdb {

class Column : public expr::Expression {
  public:
    static ColumnPtr create(SelectablePtr selectable,
                            const QString& name) {
        return ColumnPtr(new Column(selectable, name));
    }

    void selectable(SelectablePtr selectable) {
        selectable_ = selectable;
    }

    SelectablePtr selectable() const { return selectable_; }

    void name(const QString& name) {
        name_ = name;
    }

    const QString& name() const { return name_; }

  protected:
    Column(SelectablePtr selectable,
           const QString& name)
            : selectable_(selectable)
            , name_(name) {

    }

  private:
    SelectablePtr selectable_;
    QString name_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_COLUMN_HPP
