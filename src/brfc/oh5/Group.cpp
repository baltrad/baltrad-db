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

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/NodeImpl.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {
namespace oh5 {

Group::Group(auto_ptr<NodeImpl> impl)
    : Node(impl) {

}

Group::~Group() {

}

Attribute*
Group::attribute(const String& path) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->attribute(path));
}

const Attribute*
Group::attribute(const String& path) const {
    return dynamic_cast<const Attribute*>(child(path));
}

Attribute*
Group::effective_attribute(const String& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->effective_attribute(name));
}

const Attribute*
Group::effective_attribute(const String& name) const {
    const Attribute* child = attribute(name);
    if (child)
        return child;
    const Group* pgroup = parent<Group>();
    if (pgroup)
        return pgroup->effective_attribute(name);
    return 0;
}

Group*
Group::group(const String& path) {
    return dynamic_cast<Group*>(child(path));
}

const Group*
Group::group(const String& name) const {
    return dynamic_cast<const Group*>(child(name));
}

Group&
Group::get_or_create_group(const String& pathstr) {
    if (pathstr.starts_with("/") and not is_root())
        throw value_error("path must not be absolute");

    StringList path = pathstr.split("/", String::SKIP_EMPTY_PARTS);

    Group* last = this;
    Group* node = 0;

    StringList::const_iterator iter = path.begin();

    // seek existing nodes
    while (iter != path.end()) {
        node = last->group(*iter);
        if (not node)
            break;
        last = node;
        ++iter;
    }
    
    // create missing nodes
    while (iter != path.end()) {
        last = &last->impl().create_group(*(iter++));
    }

    return *last;
}

bool
Group::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const RootGroup*>(&node))
        return false;
    return true;
}

} // namespace oh5
} // namespace brfc
