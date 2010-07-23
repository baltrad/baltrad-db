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

#include <brfc/sql/Alias.hpp>

#include <boost/foreach.hpp>

#include <brfc/sql/Column.hpp>

namespace brfc {
namespace sql {

ColumnPtr
Alias::column(const String& name) const {
    ColumnPtr c = aliased_->column(name);
    c = c->proxy(this->shared_from_this());
    return c;
}

std::vector<ColumnPtr>
Alias::columns() const {
    std::vector<ColumnPtr> cols;
    BOOST_FOREACH(ColumnPtr c, aliased_->columns()) {
        cols.push_back(c->proxy(this->shared_from_this()));
    }
    return cols;
}

} // namespace sql
} // namespace brfc
