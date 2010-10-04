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

#ifndef BRFC_OH5_NODE_IMPL_HPP
#define BRFC_OH5_NODE_IMPL_HPP

#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

namespace brfc {

class String;

namespace oh5 {

class Attribute;
class Group;
class Scalar;

class Node;

/**
 * @brief ABC for Node implementations
 */
class NodeImpl : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~NodeImpl();

    /**
     * @brief add a child group
     * @throw duplicate_entry if a child with that name already exists
     * @throw value_error if Group can not be created as child
     *
     * @sa do_create_child_group
     */
    Group& create_group(const String& name);    

    /**
     * @brief add a child attribute
     * @throw duplicate_entry if a child with that name already exists
     * @throw value_error if Attribute can not be created as child
     *
     */
    Attribute& create_attribute(const String& name, const Scalar& value);

    /**
     * @brief access node name
     */
    const String& name() const { return do_name(); }

    /**
     * @brief parent node
     * @return pointer to a parent or null pointer if this node has no parent
     */
    Node* parent() {
        return do_parent();
    }
    
    /**
     * @copydoc parent()
     */
    const Node* parent() const {
        return do_parent();
    }

    Node& add_child(Node* node);

    /**
     * @brief access children
     */
    std::vector<const Node*> children() const {
        return do_children();
    }

    std::vector<Node*> children() {
        return do_children();
    }

    void front(Node* front) { front_ = front; }

    Node* front() const { return front_; }

  protected:
    virtual Group* do_create_group(const String& path) = 0;
    virtual Attribute* do_create_attribute(const String& path,
                                           const Scalar& value) = 0;

    virtual const String& do_name() const = 0;

    virtual Node* do_parent() = 0;
    virtual const Node* do_parent() const = 0;

    virtual Node& do_add_child(Node* node) = 0;

    virtual std::vector<Node*> do_children() = 0;
    virtual std::vector<const Node*> do_children() const = 0;
    
  private:
    Node* front_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_IMPL_HPP
