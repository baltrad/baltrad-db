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

#include <brfc/expr/proc/in.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {

sexp
in::operator()(const sexp& args) const{
    BRFC_ASSERT(args.size() == 2);
    sexp::const_iterator it = args.begin();
    const sexp& lhs = *it;
    ++it;
    const sexp& rhs = *it;
    BRFC_ASSERT(not lhs.is_list());
    BRFC_ASSERT(rhs.is_list());

    it = rhs.begin();
    for ( ; it != rhs.end(); ++it) {
        if (lhs == *it)
            return sexp(true);
    }
    return sexp(false);
}

} // namespace expr
} // namespace brfc