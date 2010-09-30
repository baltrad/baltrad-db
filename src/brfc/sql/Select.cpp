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

#include <brfc/sql/Select.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Expression.hpp>
#include <brfc/sql/Label.hpp>

namespace brfc {
namespace sql {

Select::Select(SelectablePtr from)
        : what_()
        , from_(from)
        , where_()
        , order_()
        , distinct_(false) {
    if (from) {
        const std::vector<ColumnPtr>& cols = from->columns();
        what_.insert(what_.end(), cols.begin(), cols.end());
    }
}

void
Select::what(ExpressionPtr expr) {
    what_.push_back(expr);
}

std::vector<ColumnPtr>
Select::columns() const {
    std::vector<ColumnPtr> cols;
    LabelPtr lbl;
    ColumnPtr col;
    BOOST_FOREACH(ExpressionPtr xpr, what_) {
        if (col = dynamic_pointer_cast<Column>(xpr))
            col = col->proxy(this->shared_from_this());
        else if (lbl = dynamic_pointer_cast<Label>(xpr)) {
            if (col = dynamic_pointer_cast<Column>(lbl->expression())) {
                col = col->proxy(lbl->name(), this->shared_from_this());
            } else {
                col = Column::create(lbl->name(), this->shared_from_this());
            }
        }
        cols.push_back(col->proxy(this->shared_from_this()));
    }
    return cols;
}

void
Select::append_where(ExpressionPtr expr) {
    where_ = where_ ? where_->and_(expr) : expr;
}

void
Select::append_order_by(ExpressionPtr expr, SortDirection dir) {
    order_.push_back(std::make_pair(expr, dir));
}

} // namespace sql
} // namespace brfc
