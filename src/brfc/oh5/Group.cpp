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

#include <brfc/oh5/Group.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

Group::~Group() {

}

shared_ptr<Attribute>
Group::child_attribute(const QString& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->child_attribute(name));
}

shared_ptr<const Attribute>
Group::child_attribute(const QString& name) const {
    return dynamic_pointer_cast<const Attribute>(child_by_path(name));
}

shared_ptr<Attribute>
Group::attribute(const QString& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->attribute(name));
}

shared_ptr<const Attribute>
Group::attribute(const QString& name) const {
    shared_ptr<const Attribute> child = child_attribute(name);
    if (child)
        return child;
    shared_ptr<const Group> pgroup = parent<Group>();
    if (pgroup)
        return pgroup->attribute(name);
    return shared_ptr<const Attribute>();
}

shared_ptr<Group>
Group::child_group_by_name(const QString& name) {
    return dynamic_pointer_cast<Group>(child_by_name(name));
}

shared_ptr<const Group>
Group::child_group_by_name(const QString& name) const {
    return dynamic_pointer_cast<const Group>(child_by_name(name));
}

bool
Group::do_accepts_parent(const Node& node) const {
    if (dynamic_cast<const Group*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
