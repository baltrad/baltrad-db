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

#ifndef BRFC_OH5_NODE_BACKEND_HPP
#define BRFC_OH5_NODE_BACKEND_HPP

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace brfc {
namespace oh5 {

class Node;

/**
 * @brief ABC for Node implementations
 */
class NodeBackend : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~NodeBackend();
    
    /**
     * @brief add a node to this backend
     * @param parent the parent node
     * @param node the node to add (ownership transfered)
     * @return reference to the added node
     *
     * @throw duplicate_entry if parent already has a child with name
     * @throw lookup_error if parent is not associated with this backend
     * 
     * @note don't rely on @c node pointer after this call. The ownership
     *       transfers, but the backend might create a copy instead. Use
     *       the returned reference instead.
     */
    Node& add(const Node& parent, Node* node);
    
    /**
     * @brief add a node to this backend
     * @param path path to parent node
     * @param node the node to add (ownership transfered)
     * @return reference to the added node
     *
     * @throw lookup_error if no parent by path
     * @sa add(const Node&, Node*)
     */
    Node& add(const std::string& path, Node* node);
    
    /**
     * @brief test if this backend owns @c node
     */
    bool has(const oh5::Node& node) const {
        return do_has(node);
    }

    /**
     * @brief access the root node
     */
    Node& root() {
        return const_cast<Node&>(const_this().do_root());
    }
    
    /**
     * @brief access the root node
     */
    const Node& root() const {
        return do_root();
    }
     
    /**
     * @brief access node by @c path
     */
    Node* node(const std::string& path) {
        return child_by_path(root(), path);
    }
    
    /**
     * @brief access node by @c path
     */
    const Node* node(const std::string& path) const {
        return child_by_path(root(), path);
    }
    
    /**
     * @brief access children of @c node
     */
    std::vector<Node*> children(const Node& node);    

    /**
     * @brief access children of @c node
     */
    std::vector<const Node*> children(const Node& node) const {
        return do_children(node);
    }
    
    /**
     * @brief access child of @c node by @c name
     */
    Node* child_by_name(const Node& node, const std::string& name) {
        return const_cast<Node*>(const_this().child_by_name(node, name));
    }

    /**
     * @brief access child of @c node by @c name
     */
    const Node* child_by_name(const Node& node,
                              const std::string& name) const;

    /**
     * @brief access child of @c node by @c path
     */
    Node* child_by_path(const Node& node, const std::string& path) {
        return const_cast<Node*>(const_this().child_by_path(node, path));
    }

    /**
     * @brief access child node of @c node by @c path
     */
    const Node* child_by_path(const Node& node,
                              const std::string& path) const;
    
    /**
     * @brief get absolute path of @c node
     */
    std::string path(const Node& node) const;

  private:
    const NodeBackend& const_this() {
        return const_cast<const NodeBackend&>(*this);
    }

    virtual Node& do_add(Node* node) = 0;

    virtual bool do_has(const Node& node) const = 0;

    virtual const Node& do_root() const = 0;

    virtual std::vector<const Node*> do_children(const Node& node) const = 0;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_BACKEND_HPP
