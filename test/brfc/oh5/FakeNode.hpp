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
#ifndef BRFC_OH5_FAKE_NODE
#define BRFC_OH5_FAKE_NODE

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

class FakeNode : public Node {
  public:
    FakeNode(const std::string& name)
            : Node(name) {
    }

  protected:
    FakeNode(const FakeNode& other)
            : Node(other) {
    }
  
  private:
    virtual bool do_accepts_child(const Node&) const { return true; }
    virtual Node* do_clone() const { return new FakeNode(*this); }
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_FAKE_NODE
