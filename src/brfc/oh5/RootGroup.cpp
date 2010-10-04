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

#include <brfc/oh5/RootGroup.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/NodeImpl.hpp>

namespace brfc {
namespace oh5 {

RootGroup::~RootGroup() { }

RootGroup::RootGroup(NodeImpl* impl, File* file)
        : Group(impl)
        , file_(file) {

}

bool
RootGroup::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const RootGroup*>(&node) != 0) {
        return false;
    } else if (dynamic_cast<const Group*>(&node) != 0 or
        dynamic_cast<const Attribute*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
