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

#ifndef BRFC_RDB_FWD_HPP
#define BRFC_RDB_FWD_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

/**
 * @brief relational database & SQL
 */
namespace rdb {

class Alias;
class Column;
class FromClause;
class Join;
class Selectable;
class Select;
class Table;

typedef shared_ptr<Alias> AliasPtr;
typedef shared_ptr<Column> ColumnPtr;
typedef shared_ptr<FromClause> FromClausePtr;
typedef shared_ptr<Join> JoinPtr;
typedef shared_ptr<Selectable> SelectablePtr;
typedef shared_ptr<Select> SelectPtr;
typedef shared_ptr<Table> TablePtr;

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_FWD_HPP
