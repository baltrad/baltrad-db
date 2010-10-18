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

#ifndef BRFC_OH5_NODE_BACKEND_HPP
#define BRFC_OH5_NODE_BACKEND_HPP

#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

namespace brfc {

class String;

namespace oh5 {

class Node;

/**
 * @brief ABC for Node implementations
 */
class NodeBackend : public boost::noncopyable {
  public:
    explicit NodeBackend(Node* front=0)
            : front_(front) {
    }

    /**
     * @brief destructor
     */
    virtual ~NodeBackend();

    void front(Node* front) { front_ = front; }

    Node* front() const { return front_; }
    
    /**
     * @brief add a child node, creating it on backend
     *
     * ownership of @c node is transfered
     */
    Node& create_child(Node* node);

    std::vector<Node*> children() { return do_children(); }
    std::vector<const Node*> children() const { return do_children(); }

  protected:
    virtual Node& do_create_child(Node* node) = 0;

    virtual std::vector<Node*> do_children() = 0;
    virtual std::vector<const Node*> do_children() const = 0;
    
  private:
    Node* front_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_BACKEND_HPP
