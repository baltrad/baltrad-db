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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/expr/Join.hpp>

#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

Join::Join(SelectablePtr from,
           SelectablePtr to,
           ExpressionPtr condition,
           Type type)
        : from_(from)
        , to_(to)
        , condition_(condition)
        , type_(type) {

}

void
Join::do_accept(Visitor& visitor) {
    visitor.visit(*this);
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

} // namespace expr
} // namespace brfc
