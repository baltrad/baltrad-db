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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/rdb/Selectable.hpp>

#include <brfc/rdb/Alias.hpp>
#include <brfc/rdb/Column.hpp>
#include <brfc/rdb/Join.hpp>

namespace brfc {
namespace rdb {

AliasPtr
Selectable::alias(const QString& name) {
    return Alias::create(this->shared_from_this(), name);
}

ColumnPtr
Selectable::column(const QString& name) {
    return Column::create(this->shared_from_this(), name);
}

JoinPtr
Selectable::join(SelectablePtr rhs, expr::ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::INNER);
}

JoinPtr
Selectable::outerjoin(SelectablePtr rhs, expr::ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::LEFT);
}

} // namespace rdb
} // namespace brfc
