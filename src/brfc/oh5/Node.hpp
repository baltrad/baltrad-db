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

#ifndef BRFC_OH5_NODE_HPP
#define BRFC_OH5_NODE_HPP

#include <deque>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <brfc/smart_ptr.hpp>
#include <string>

namespace brfc {
namespace oh5 {

class Attribute;
class DataSet;
class File;
class Group;
class NodeBackend;
class Scalar;

template<typename T>
class NodeIterator;

/**
 * @brief ABC for all ODIM_H5 elements
 *
 * A node is a single element in the ODIM_H5 hirerchy. It has a name which
 * is unique among the children of one parent.
 */
class Node : public boost::noncopyable {
  public:
    typedef NodeIterator<Node> iterator;
    typedef NodeIterator<const Node> const_iterator;

    /**
     * @brief constructor
     * @param name name of this node
     *
     * the node has no parent and no backend
     */
    Node(const std::string& name);

    /**
     * @brief destructor
     */
    virtual ~Node();

    bool has_backend() const { return backend_.get() != 0; }
    
    NodeBackend& backend();
    const NodeBackend& backend() const;
    
    /**
     * @brief associate with backend
     *
     * ownership of @c backend is transfered
     * backend.front() is set to this node
     */
    void backend(NodeBackend* backend);
    
    /**
     * @brief access node name
     */
    const std::string& name() const { return name_; }
    
    /**
     * @brief absolute path of this node
     *
     * path is the concatenation of all node names leading from root to
     * this node, separated by "/".
     */
    std::string path() const;

    /**
     * @brief parent node
     * @return pointer to a parent or null pointer if this node has no parent
     */
    Node* parent() { return parent_; }
    
    /**
     * @copydoc parent()
     */
    const Node* parent() const { return parent_; }

    void parent(Node* parent) { parent_ = parent; }
    
    /**
     * @brief parent node of type T
     * @tparam T type to test parent against
     * @return pointer to a parent node of type T or null pointer if this
     *         node has no parent or parent is not of type T
     */
    template<typename T>
    T* parent() {
        return dynamic_cast<T*>(parent());
    }

    /**
     * @brief parent node of type T
     * @tparam T type to test parent against
     * @return pointer to a parent node of type T or null pointer if this
     *         node has no parent or parent is not of type T
     */
    template<typename T>
    const T* parent() const {
        return dynamic_cast<const T*>(parent());
    }

    /**
     * @brief is this node the root node
     */
    bool is_root() const {
        return parent() == 0;
    }
    
    /**
     * @brief get root node
     */
    Node& root();
    
    /**
     * @copydoc root()
     */
    const Node& root() const;
    
    /**
     * @brief create a child attribute
     * @param name attribute name
     * @param value attribute value
     * @return reference to the created attribute
     */
    Attribute& create_attribute(const std::string& name, const Scalar& value);
    
    /**
     * @brief create a child dataset
     * @param name dataset name
     * @return reference to the created dataset
     */
    DataSet& create_dataset(const std::string& name);
    
    /**
     * @brief create a child group
     * @param name group name
     * @return reference to the created group
     */
    Group& create_group(const std::string& name);
    
    /**
     * @brief create a child node
     */
    Node& create_child(Node* node);

    /**
     * @brief test for a child node
     * @param path path to the child, relative to this node
     * @throw value_error if the path is absolute and this node is not root
     * @return true if the child exists
     */
    bool has_child(const std::string& path) const;

    /**
     * @brief access child node at @c path
     * @param path path to the child, relative to this node
     * @throw value_error if the path is absolute and this node is not root
     * @return pointer to Node or null if not found.
     */
    Node* child(const std::string& path);
    
    /**
     * @copydoc child()
     */
    const Node* child(const std::string& path) const;

    /**
     * @brief test if this node can have @c node as a child
     * @sa do_accepts_child
     */
    bool accepts_child(const Node& node) const;
    
    /**
     * @brief access children
     *
     * forwards to NodeBackend::children()
     */
    std::vector<const Node*> children() const;
    
    /**
     * @brief access children
     *
     * forwards to NodeBackend::children() const
     */
    std::vector<Node*> children();

    /**
     * @brief file this node is associated with
     * @return pointer to File or null if not associated with a file
     *
     * default implementation returns the file associated with root and
     * for root returns a null pointer.
     */
    File* file() {
        const Node* self = const_cast<const Node*>(this);
        return const_cast<File*>(self->file());
    }

    /**
     * @copydoc file()
     */
    const File* file() const {
        return do_file();
    }
        
    iterator begin();
    const_iterator begin() const;
    
    iterator end();
    const_iterator end() const;

  protected:
    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual const File* do_file() const;

  private:
    std::string name_;
    Node* parent_;
    scoped_ptr<NodeBackend> backend_;
};

template<typename T>
class NodeIterator :
        public boost::iterator_facade<NodeIterator<T>,
                                      T,
                                      boost::forward_traversal_tag> {
  public:
    explicit NodeIterator(T* node=0);

    template<typename OtherT>
    NodeIterator(const NodeIterator<OtherT>& other);

  private:
    friend class boost::iterator_core_access;
    template<typename>
    friend class NodeIterator;
    
    void increment();

    T& dereference() const;

    template<typename OtherT>
    bool equal(const NodeIterator<OtherT>& rhs) const;

    std::deque<T*> stack_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_HPP
