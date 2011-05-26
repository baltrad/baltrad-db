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

#include <brfc/expr/proc/make_procedure.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace proc {

make_procedure::make_procedure(const std::string& name, int nargs)
        : name_(name)
        , nargs_(nargs) {
}

Expression
make_procedure::operator()(const Expression& x) const {
    BRFC_ASSERT(x.is_list());
    if (nargs_ >= 0) {
        BRFC_ASSERT(x.size() == (unsigned int)nargs_);
    }
    return Listcons().symbol(name_).extend(x).get();
}

} // namespace proc
} // namespace brfc
