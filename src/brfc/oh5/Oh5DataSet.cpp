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

#include <brfc/oh5/Oh5DataSet.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>

namespace brfc {

Oh5DataSet::Oh5DataSet(const std::string& name)
        : Oh5Node(name) { 

}

Oh5DataSet::Oh5DataSet(const Oh5DataSet& other)
        : Oh5Node(other) {

}

Oh5DataSet::~Oh5DataSet() {

}

bool
Oh5DataSet::do_accepts_child(const Oh5Node& node) const {
    if (dynamic_cast<const Oh5Attribute*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

Oh5Node*
Oh5DataSet::do_clone() const {
    return new Oh5DataSet(*this);
}

} // namespace brfc
