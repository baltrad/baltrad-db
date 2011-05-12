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

#include <brfc/expr/proc/unary_list.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace expr {
namespace proc {

Expression
unary_list::operator()(const Expression& args) const {
    BRFC_ASSERT(args.size() == 1);

    if (args.front().is_list()) {
        Expression::const_iterator it = args.front().begin();
        Expression result;
        for ( ; it != args.front().end(); ++it) {
            result.push_back(proc_(Listcons().append(*it).get()));
        }
        return result;
    } else {
        return proc_(Listcons().append(args.front()).get());
    }
}

} // namepsace proc
} // namespace expr
} // namespace brfc
