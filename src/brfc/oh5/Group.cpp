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

#include <brfc/RegExp.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/QualityGroup.hpp>

namespace brfc {
namespace oh5 {

Group::~Group() {

}

shared_ptr<Group>
Group::create_by_name(const String& name) {
    RegExp re("([a-z]+)([0-9]+)");
    
    if (re.match(name)) {
        const String& grp_name = re.group(1);
        if (grp_name == "dataset") {
            return make_shared<DataSetGroup>(name);
        } else if (grp_name == "data") {
            return make_shared<DataGroup>(name);
        } else if (grp_name == "quality") {
            return make_shared<QualityGroup>(name);
        }
    }

    if (name == "what" or name == "where" or name == "how")
        return make_shared<AttributeGroup>(name);

    return shared_ptr<Group>();
}

shared_ptr<Attribute>
Group::child_attribute(const String& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->child_attribute(name));
}

shared_ptr<const Attribute>
Group::child_attribute(const String& name) const {
    return dynamic_pointer_cast<const Attribute>(child_by_path(name));
}

shared_ptr<Attribute>
Group::attribute(const String& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->attribute(name));
}

shared_ptr<const Attribute>
Group::attribute(const String& name) const {
    shared_ptr<const Attribute> child = child_attribute(name);
    if (child)
        return child;
    shared_ptr<const Group> pgroup = parent<Group>();
    if (pgroup)
        return pgroup->attribute(name);
    return shared_ptr<const Attribute>();
}

shared_ptr<Group>
Group::child_group_by_name(const String& name) {
    return dynamic_pointer_cast<Group>(child_by_name(name));
}

shared_ptr<const Group>
Group::child_group_by_name(const String& name) const {
    return dynamic_pointer_cast<const Group>(child_by_name(name));
}

shared_ptr<Group>
Group::get_or_create_child_group_by_name(const String& name) {
    shared_ptr<Group> child = child_group_by_name(name);
    if (not child) {
        child = create_by_name(name);
        if (child)
            add_child(child);
    }
    return child;
}

shared_ptr<Group>
Group::get_or_create_child_group_by_path(const StringList& path) {
    shared_ptr<Group> last_existing = shared_from_this();
    shared_ptr<Group> node;

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
        return last_existing;
    
    shared_ptr<Group> created_root = create_by_name(*(iter++));
    shared_ptr<Group> last_created = created_root;

    // create missing nodes
    while (last_created and iter != path.end()) {
        node = create_by_name(*iter);
        if (node)
            last_created->add_child(node);
        last_created = node;
        ++iter;
    }

    // all children created, attach created nodes to last pre-existing node
    if (last_created)
        last_existing->add_child(created_root);
    return last_created;
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
