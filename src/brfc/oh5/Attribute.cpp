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

#include <brfc/oh5/Attribute.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/NodeImpl.hpp>
#include <brfc/oh5/Scalar.hpp>

namespace brfc {
namespace oh5 {

Attribute::Attribute(auto_ptr<NodeImpl> impl, const Scalar& value)
        : Node(impl)
        , value_(value) {
}

Attribute::~Attribute() {

}

Group*
Attribute::parent_group() {
    const Attribute* self = const_cast<const Attribute*>(this);
    return const_cast<Group*>(self->parent_group());
}

const Group*
Attribute::parent_group() const {
    const AttributeGroup* parent_p = parent<AttributeGroup>();
    if (parent_p)
        return dynamic_cast<const Group*>(parent_p->parent());
    else
        return dynamic_cast<const Group*>(parent());
}

String
Attribute::full_name() const {
    const AttributeGroup* grp = parent<AttributeGroup>();
    if (grp) {
        return grp->name() + "/" + name();
    } else {
        return name();
    }
}

} // namespace oh5
} // namespace brfc
