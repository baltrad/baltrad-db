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

#include <brfc/sql/Table.hpp>
#include <brfc/sql/Column.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace sql {

ColumnPtr
Table::add_column(const std::string& name) {
    ColumnPtr column = Column::create(name, this->shared_from_this());
    if (not columns_.insert(std::make_pair(name, column)).second)
        throw duplicate_entry("duplicate column: " + name);
    return column;
}

ColumnPtr
Table::column(const std::string& name) const {
    ColumnMap::const_iterator iter = columns_.find(name);
    if (iter == columns_.end())
        throw lookup_error("no such column: " + name_ + "." + name);
    return iter->second;
}

std::vector<ColumnPtr>
Table::columns() const {
    std::vector<ColumnPtr> cols;
    ColumnMap::const_iterator iter = columns_.begin();
    for ( ; iter != columns_.end(); ++iter)
        cols.push_back(iter->second);
    return cols;
}

} // namespace sql
} // namespace brfc
