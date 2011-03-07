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

#include <brfc/oh5/MemoryNodeBackend.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {
namespace oh5 {

MemoryNodeBackend::MemoryNodeBackend(Node* front)
        : NodeBackend(front)
        , children_() {
}

MemoryNodeBackend::~MemoryNodeBackend() {

}

Node&
MemoryNodeBackend::do_create_child(Node* _node) {
    auto_ptr<Node> node(_node);
    node->backend(new MemoryNodeBackend());
    children_.push_back(node);
    return children_.back();
}

std::vector<const Node*>
MemoryNodeBackend::do_children() const {
    std::vector<const Node*> vec;
    BOOST_FOREACH(const Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

std::vector<Node*>
MemoryNodeBackend::do_children() {
    std::vector<Node*> vec;
    BOOST_FOREACH(Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

} // namespace oh5
} // namespace brfc

