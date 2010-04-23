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

#include <brfc/rdb/FromClause.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/rdb/Selectable.hpp>

namespace brfc {
namespace rdb {

void
FromClause::add(SelectablePtr selectable) {
    if (has(selectable))
        throw duplicate_entry("duplicate selectable");
    elements_.push_back(selectable);
}

bool
FromClause::has(SelectablePtr selectable) const {
    // always accept unnamed
    if (selectable->name() == "")
        return false;
    BOOST_FOREACH(SelectablePtr element, elements_) {
        if (selectable->name() == element->name())
            return true;
    }
    return false;
}

} // namespace rdb
} // namespace brfc
