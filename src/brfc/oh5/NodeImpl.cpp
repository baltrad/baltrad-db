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

#include <brfc/oh5/NodeImpl.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {
namespace oh5 {

NodeImpl::~NodeImpl() {

}

Group&
NodeImpl::create_group(const String& name) {
    auto_ptr<Node> node(do_create_group(name));
    return static_cast<Group&>(add_child(node.release()));
}

Attribute&
NodeImpl::create_attribute(const String& name, const Scalar& value) {
    auto_ptr<Node> node(do_create_attribute(name, value));
    return static_cast<Attribute&>(add_child(node.release()));
}

Node&
NodeImpl::add_child(Node* _node) {
    auto_ptr<Node> node(_node);
    const String& name = node->name();

    if (name.contains("/"))
        throw value_error("invalid node name: " + name.to_std_string());

    if (name == "" and not dynamic_cast<RootGroup*>(node.get()))
        throw value_error("invalid_name");

    if (not front()->accepts_child(*node))
        throw value_error("node not accepted as child");

    BOOST_FOREACH(const Node* child, children()) {
        if (child->name() == name)
            throw duplicate_entry(name.to_std_string());
    }

    return do_add_child(node.release());
}

} // namespace oh5
} // namespace brfc
