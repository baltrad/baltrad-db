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

#include <brfc/expr/ExpressionList.hpp>

#include <boost/foreach.hpp>

namespace brfc {
namespace expr {

ExpressionList::ExpressionList(const ExpressionList& other)
        : exprs_() {
    exprs_.reserve(other.exprs_.size());
    BOOST_FOREACH(ExpressionPtr expr, other.exprs_) {
        exprs_.push_back(expr->clone());
    }
}

void
ExpressionList::append(const Expression& expr) {
    exprs_.push_back(expr.clone());
}

bool
ExpressionList::do_equals(const Expression& other) const {
    const ExpressionList* optr = dynamic_cast<const ExpressionList*>(&other);
    if (optr and exprs_.size() == optr->exprs_.size()) {
        for (size_t i = 0; i < exprs_.size(); ++i) {
            if (not exprs_.at(i)->equals(*optr->exprs_.at(i)))
                return false;
        }
        return true;
    }
    return false;
}

} // namespace expr
} // namespace brfc
