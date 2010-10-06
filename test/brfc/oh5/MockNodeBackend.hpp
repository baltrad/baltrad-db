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

#ifndef BRFC_OH5_MOCK_NODE_BACKEND_HPP
#define BRFC_OH5_MOCK_NODE_BACKEND_HPP

#include <gmock/gmock.h>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/NodeBackend.hpp>

namespace brfc {
namespace oh5 {

class MockNodeBackend : public NodeBackend {
  public:
    MOCK_METHOD1(do_create_group, Group*(const String&));
    MOCK_METHOD2(do_create_attribute,
                 Attribute*(const String&, const Scalar&));
    MOCK_CONST_METHOD0(do_name, String&());
    MOCK_METHOD0(do_parent, Node*());
    MOCK_CONST_METHOD0(do_parent, Node*());
    MOCK_METHOD1(do_add_child, Node&(Node* node));
    MOCK_METHOD0(do_children, std::vector<Node*>());
    MOCK_CONST_METHOD0(do_children, std::vector<const Node*>());
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_MOCK_NODE_BACKEND_HPP
