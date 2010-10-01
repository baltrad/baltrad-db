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

#ifndef BRFC_OH5_MEMORY_NODE_IMPL_HPP
#define BRFC_OH5_MEMORY_NODE_IMPL_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/smart_ptr.hpp>
#include <brfc/String.hpp>

#include <brfc/oh5/NodeImpl.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief NodeImpl storing nodes in memory
 */
class MemoryNodeImpl : public NodeImpl {
  public:
    /**
     * @brief constructor
     * @param name name of this node
     * @throw value_error if name contains "/"
     */
    explicit MemoryNodeImpl(const String& name);

    virtual ~MemoryNodeImpl();
  
  protected:
    virtual Group& do_create_group(const String& name);
    virtual Attribute& do_create_attribute(const String& name,
                                           const Scalar& value);

    virtual const String& do_name() const { return name_; }

    virtual Node* do_parent() { return parent_; }
    virtual const Node* do_parent() const { return parent_; }

    virtual std::vector<const Node*> do_children() const;
    virtual std::vector<Node*> do_children();

  private:
    Node& add_child(auto_ptr<Node> node);

    String name_;
    Node* parent_;
    boost::ptr_vector<Node> children_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_MEMORY_NODE_IMPL_HPP