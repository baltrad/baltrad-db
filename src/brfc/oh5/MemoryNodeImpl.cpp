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

#include <brfc/oh5/MemoryNodeImpl.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {
namespace oh5 {

MemoryNodeImpl::MemoryNodeImpl(const String& name)
        : name_(name)
        , parent_(0)
        , children_() {
    if (name.contains("/"))
        throw value_error("invalid node name: " + name.to_std_string());
}

MemoryNodeImpl::~MemoryNodeImpl() {

}

Group&
MemoryNodeImpl::do_create_group(const String& name) {
    auto_ptr<NodeImpl> impl(new MemoryNodeImpl(name));

    auto_ptr<Node> group;
    if (name == "what" or name == "where" or name == "how") {
        group.reset(new AttributeGroup(impl));
    } else {
        group.reset(new Group(impl));
    }

    return static_cast<Group&>(add_child(group));
}

Attribute&
MemoryNodeImpl::do_create_attribute(const String& name,
                                    const Scalar& value) {
    auto_ptr<NodeImpl> impl(new MemoryNodeImpl(name));
    auto_ptr<Node> attr(new Attribute(impl, value));

    return static_cast<Attribute&>(add_child(attr));
}

Node&
MemoryNodeImpl::add_child(auto_ptr<Node> node) {
    const String& name = node->name();

    BOOST_FOREACH(const Node& child, children_) {
        if (child.name() == name)
            throw duplicate_entry(name.to_std_string());
    }

    if (name == "" and not dynamic_cast<RootGroup*>(node.get()))
        throw value_error("invalid_name");
    if (not front()->accepts_child(*node))
        throw value_error("node not accepted as child");

    static_cast<MemoryNodeImpl&>(node->impl()).parent_ = front();
    children_.push_back(node);
    return children_.back();
}

std::vector<const Node*>
MemoryNodeImpl::do_children() const {
    std::vector<const Node*> vec;
    BOOST_FOREACH(const Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

std::vector<Node*>
MemoryNodeImpl::do_children() {
    std::vector<Node*> vec;
    BOOST_FOREACH(Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

} // namespace oh5
} // namespace brfc

