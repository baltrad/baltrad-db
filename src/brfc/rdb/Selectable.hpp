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

#ifndef BRFC_RDB_SELECTABLE_HPP
#define BRFC_RDB_SELECTABLE_HPP

#include <QtCore/QString>

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Element.hpp>

#include <brfc/rdb/fwd.hpp>

namespace brfc {
namespace rdb {

class Selectable : public expr::Element {
  public:
    SelectablePtr shared_from_this() const {
        return static_pointer_cast<Selectable>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }

    virtual QString name() const = 0;

    ColumnPtr column(const QString& name);

    AliasPtr alias(const QString& name);

    JoinPtr join(SelectablePtr rhs, expr::ExpressionPtr condition);

    JoinPtr outerjoin(SelectablePtr rhs, expr::ExpressionPtr condition);
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SELECTABLE_HPP