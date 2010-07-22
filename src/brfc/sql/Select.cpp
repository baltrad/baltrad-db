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
#include <brfc/sql/FromClause.hpp>
#include <brfc/sql/Label.hpp>

namespace brfc {
namespace sql {

Select::Select()
        : what_()
        , from_(FromClause::create())
        , where_()
        , distinct_(false) {
}

void
Select::what(ExpressionPtr expr) {
    what_.push_back(expr);
}

ColumnPtr
Select::column(const String& name) const {
    ColumnPtr found;
    LabelPtr lbl;
    BOOST_FOREACH(ExpressionPtr xpr, what_) {
        if (xpr->name() != name)
            continue;
        if (found)
            throw lookup_error("ambiguous: " + name.to_std_string());

        if (found = dynamic_pointer_cast<Column>(xpr)) {
            found = found->rebase(this->shared_from_this());
        } else if (lbl = dynamic_pointer_cast<Label>(xpr)) {
            found = Column::create(lbl->name(), this->shared_from_this());
        }
    }

    if (not found)
        throw lookup_error("not found:" + name.to_std_string());
    return found;
}

std::vector<ColumnPtr>
Select::columns() const {
    std::vector<ColumnPtr> cols;
    LabelPtr lbl;
    ColumnPtr col;
    BOOST_FOREACH(ExpressionPtr xpr, what_) {
        if (col = dynamic_pointer_cast<Column>(xpr))
            cols.push_back(col->rebase(this->shared_from_this()));
        else if (lbl = dynamic_pointer_cast<Label>(xpr))
            cols.push_back(Column::create(lbl->name(),
                                          this->shared_from_this()));
    }
    return cols;
}

void
Select::append_where(ExpressionPtr expr) {
    where_ = where_ ? where_->and_(expr) : expr;
}

} // namespace sql
} // namespace brfc
