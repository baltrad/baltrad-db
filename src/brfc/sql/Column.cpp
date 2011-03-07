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

#include <brfc/sql/Column.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace sql {

void
Column::references(ColumnPtr column) {
    if (references_ or parent_)
        throw value_error("can't change reference");
    references_ = column;
}

bool
Column::is_proxy_of(const Column& col) const {
    if (parent_) {
        if (col == (*parent_))
            return true;
        return parent_->is_proxy_of(col);
    }
    return false;
}

bool
Column::operator==(const Column& rhs) const {
    return name_ == rhs.name_ and selectable_ == rhs.selectable_;
}

ColumnPtr
Column::proxy(SelectablePtr t) const {
    return proxy(name(), t);
}

ColumnPtr
Column::proxy(const std::string& name, SelectablePtr t) const {
    return Column::create(name, t, this->shared_from_this());
}

} // namespace sql
} // namespace brfc
