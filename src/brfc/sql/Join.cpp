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

#include <brfc/sql/Join.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

Join::Join(SelectablePtr from,
           SelectablePtr to,
           ExpressionPtr condition,
           Type type)
        : from_(from)
        , to_(to)
        , condition_(condition)
        , type_(type) {
    if (type_ != CROSS and not condition_)
        condition_ = find_condition(*from, *to);
}

ExpressionPtr
Join::find_condition(const Selectable& lhs, const Selectable& rhs) {

    ColumnPtr fk_match;
    BOOST_FOREACH(ColumnPtr col, lhs.fk_columns()) {
        if (fk_match = rhs.matching_column(*col->references())) {
            return col->eq(fk_match);
        }
    }

    BOOST_FOREACH(ColumnPtr col, rhs.fk_columns()) {
        if (fk_match = lhs.matching_column(*col->references())) {
            return col->eq(fk_match);
        }
    }

    throw value_error("can't determine join");
}

bool
Join::contains(SelectablePtr element) const {
    int contains = 0;

    if (JoinPtr j = dynamic_pointer_cast<Join>(from_)) {
        contains += j->contains(element);
    } else {
        contains += from_->name() == element->name();
    }

    if (JoinPtr j = dynamic_pointer_cast<Join>(to_)) {
        contains += j->contains(element);
    } else {
        contains += to_->name() == element->name();
    }

    return contains;
}

std::vector<ColumnPtr>
Join::columns() const {
    std::vector<ColumnPtr> cols = from_->columns();
    const std::vector<ColumnPtr>& to_cols = to_->columns();
    cols.insert(cols.end(), to_cols.begin(), to_cols.end());
    return cols;
}

} // namespace sql
} // namespace brfc
