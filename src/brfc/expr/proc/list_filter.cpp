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
#include <brfc/expr/proc/list_filter.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace expr {
namespace proc {

Expression
list_filter::operator()(const Expression& args) const {
    BRFC_ASSERT(args.is_list());
    BRFC_ASSERT(args.size() == 2);
    
    Expression::const_iterator it = args.begin();
    const Expression& lhs = *it;
    ++it;
    const Expression& rhs = *it;
    Expression::type::_ ltype = lhs.type();
    Expression::type::_ rtype = rhs.type();
    if (ltype == Expression::type::LIST and
        rtype == Expression::type::LIST) {
        return both_list(lhs, rhs);
    } else if (ltype == Expression::type::LIST and
               rtype != Expression::type::LIST) {
        return left_list(lhs, rhs);
    } else if (ltype != Expression::type::LIST and
               rtype == Expression::type::LIST) {
        return right_list(lhs, rhs);
    } else {
        return proc_(Listcons().append(lhs).append(rhs).get());
    }
}

Expression
list_filter::left_list(const Expression& lhs,
                       const Expression& rhs) const {
    BRFC_ASSERT(lhs.is_list());
    BRFC_ASSERT(not rhs.is_list());

    Expression::const_iterator it = lhs.begin();
    for ( ; it != lhs.end(); ++it) {
        if (proc_(Listcons().append(*it).append(rhs).get())) {
            return Expression(true);
        }
    }
    return Expression(false);
}

Expression
list_filter::right_list(const Expression& lhs,
                        const Expression& rhs) const {
    BRFC_ASSERT(not lhs.is_list());
    BRFC_ASSERT(rhs.is_list());

    Expression::const_iterator it = rhs.begin();
    for ( ; it != rhs.end(); ++it) {
        if (proc_(Listcons().append(lhs).append(*it).get())) {
            return Expression(true);
        }
    }
    return Expression(false);
}

Expression
list_filter::both_list(const Expression& lhs,
                       const Expression& rhs) const {
    BRFC_ASSERT(lhs.is_list());
    BRFC_ASSERT(rhs.is_list());

    Expression::const_iterator lit = lhs.begin();
    for ( ; lit != lhs.end(); ++lit) {
        Expression::const_iterator rit = rhs.begin();
        for ( ; rit != rhs.end(); ++rit) {
            if (proc_(Listcons().append(*lit).append(*rit).get())) {
                return Expression(true);
            }
        }
    }
    return Expression(false);
}

} // namepsace proc
} // namespace expr
} // namespace brfc
