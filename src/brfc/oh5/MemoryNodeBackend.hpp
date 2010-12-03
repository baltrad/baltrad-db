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

#ifndef BRFC_OH5_MEMORY_NODE_BACKEND_HPP
#define BRFC_OH5_MEMORY_NODE_BACKEND_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/smart_ptr.hpp>
#include <string>

#include <brfc/oh5/NodeBackend.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief NodeBackend storing nodes in memory
 */
class MemoryNodeBackend : public NodeBackend {
  public:
    /**
     * @brief constructor
     * @param name name of this node
     * @throw value_error if name contains "/"
     */
    explicit MemoryNodeBackend(Node* front=0);

    virtual ~MemoryNodeBackend();
  
  protected:
    virtual Node& do_create_child(Node* node);

    virtual std::vector<const Node*> do_children() const;
    virtual std::vector<Node*> do_children();

  private:
    boost::ptr_vector<Node> children_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_MEMORY_NODE_BACKEND_HPP
