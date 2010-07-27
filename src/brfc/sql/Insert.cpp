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

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

void
Insert::value(const String& column, ExpressionPtr expr) {
    value(table_->column(column), expr);
}

void
Insert::value(ColumnPtr column, ExpressionPtr expr) {
    if (column->selectable() != table_)
        throw lookup_error("table mismatch");
    BOOST_FOREACH(const ValueMap::value_type& val, values_) {
        if (val.first == column)
            throw duplicate_entry(column->name().to_std_string());
    }
    values_.push_back(std::make_pair(column, expr));
}

void
Insert::add_return(ExpressionPtr expr) {
    returns_.push_back(expr);
}

} // namespace sql
} // namespace brfc
