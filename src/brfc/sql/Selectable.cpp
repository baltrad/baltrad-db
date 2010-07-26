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

#include <brfc/sql/Selectable.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Join.hpp>

namespace brfc {
namespace sql {

AliasPtr
Selectable::alias(const String& name) {
    return Alias::create(this->shared_from_this(), name);
}

JoinPtr
Selectable::join(SelectablePtr rhs, ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::INNER);
}

JoinPtr
Selectable::outerjoin(SelectablePtr rhs, ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::LEFT);
}

JoinPtr
Selectable::crossjoin(SelectablePtr rhs) {
    return Join::create(this->shared_from_this(), rhs, ExpressionPtr(), Join::CROSS);
}

ColumnPtr
Selectable::column(const String& name) const {
    ColumnPtr found;
    BOOST_FOREACH(ColumnPtr col, columns()) {
        if (col->name() == name) {
            if (found)
                throw lookup_error("ambiguous: " + name.to_std_string());
            found = col;
        }
    }
    if (not found)
        throw lookup_error(name.to_std_string());
    return found;
}

std::vector<ColumnPtr>
Selectable::fk_columns() const {
    std::vector<ColumnPtr> cols;
    BOOST_FOREACH(ColumnPtr col, columns()) {
        if (col->references())
            cols.push_back(col);
    }
    return cols;
}

ColumnPtr
Selectable::matching_column(const Column& column) const {
    BOOST_FOREACH(ColumnPtr col, columns()) {
        if (*col == column or col->has_parent(column))
            return col;
    }
    return ColumnPtr();
}

} // namespace sql
} // namespace brfc