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

#include <brfc/sql/Insert.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

void
Insert::value(const String& column, ExpressionPtr expr) {
    // propagate lookup_error
    table_->column(column);
    if (not values_.insert(std::make_pair(column, expr)).second)
        throw duplicate_entry(column.to_std_string());
}

void
Insert::return_(ExpressionPtr expr) {
    returns_.push_back(expr);
}

} // namespace sql
} // namespace brfc
