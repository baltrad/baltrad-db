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

#include <brfc/oh5/Oh5Attribute.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

namespace brfc {

Oh5Attribute::Oh5Attribute(const std::string& name, const Oh5Scalar& value)
        : Oh5Node(name)
        , value_(value) {
}

Oh5Attribute::Oh5Attribute(const Oh5Attribute& other)
        : Oh5Node(other)
        , value_(other.value_) {

}

Oh5Attribute::~Oh5Attribute() {

}

std::string
Oh5Attribute::full_name() const {
    const Oh5Group* grp = parent<Oh5Group>();
    if (grp) {
        const std::string& grpname = grp->name();
        if (grpname == "what" or grpname == "where" or grpname == "how")
            return grpname + "/" + name();
    }
    return name();
}

Oh5Node*
Oh5Attribute::do_clone() const {
    return new Oh5Attribute(*this);
}

} // namespace brfc
