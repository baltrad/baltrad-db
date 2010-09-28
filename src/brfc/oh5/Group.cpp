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

#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

Group::~Group() {

}

auto_ptr<Group>
Group::create_by_name(const String& name) {
    auto_ptr<Group> node;
    if (name == "what" or name == "where" or name == "how") {
        node.reset(new AttributeGroup(0, name));
    } else {
        node.reset(new Group(0, name));
    }
    return node;
}

Attribute*
Group::child_attribute(const String& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->child_attribute(name));
}

const Attribute*
Group::child_attribute(const String& name) const {
    return dynamic_cast<const Attribute*>(child_by_path(name));
}

Attribute*
Group::attribute(const String& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->attribute(name));
}

const Attribute*
Group::attribute(const String& name) const {
    const Attribute* child = child_attribute(name);
    if (child)
        return child;
    const Group* pgroup = parent<Group>();
    if (pgroup)
        return pgroup->attribute(name);
    return 0;
}

Group*
Group::child_group_by_name(const String& name) {
    return dynamic_cast<Group*>(child_by_name(name));
}

const Group*
Group::child_group_by_name(const String& name) const {
    return dynamic_cast<const Group*>(child_by_name(name));
}

Group&
Group::get_or_create_child_group_by_name(const String& name) {
    Group* child = child_group_by_name(name);
    if (not child) {
        child = &create_child_group(name);
    }
    return *child;
}

Group&
Group::get_or_create_child_group_by_path(const StringList& path) {
    Group* last_existing = this;
    Group* node = 0;

    StringList::const_iterator iter = path.begin();

    // seek existing nodes
    while (iter != path.end()) {
        node = last_existing->child_group_by_name(*iter);
        if (not node)
            break;
        last_existing = node;
        ++iter;
    }
    
    // no nodes need to be created
    if (iter == path.end())
        return *last_existing;
    
    auto_ptr<Group> created_root(create_by_name(*(iter++)));
    Group* last_created = created_root.get();

    // create missing nodes
    while (last_created and iter != path.end()) {
        auto_ptr<Node> n(create_by_name(*iter));
        last_created = &static_cast<Group&>(last_created->add_child(n));
        ++iter;
    }

    // all children created, attach created nodes to last pre-existing node
    if (last_created) {
        auto_ptr<Node> n(created_root);
        last_existing->add_child(n);
    }
    return *last_created;
}

bool
Group::do_accepts_child(const Node& node) const {
    return true;
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
