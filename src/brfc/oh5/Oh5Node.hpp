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

#ifndef BRFC_OH5_NODE_HPP
#define BRFC_OH5_NODE_HPP

#include <deque>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace brfc {
namespace oh5 {

class Oh5Attribute;
class Oh5DataSet;
class Oh5File;
class Oh5Group;
class Oh5NodeBackend;
class Oh5Scalar;

template<typename T>
class Oh5NodeIterator;

/**
 * @brief ABC for all ODIM_H5 elements
 *
 * A node is a single element in the ODIM_H5 hirerchy. It has a name which
 * is unique among the children of one parent.
 */
class Oh5Node : public boost::noncopyable {
  public:
    typedef Oh5NodeIterator<Oh5Node> iterator;
    typedef Oh5NodeIterator<const Oh5Node> const_iterator;

    /**
     * @brief constructor
     * @param name name of this node
     *
     * the node has no parent and no backend
     */
    explicit Oh5Node(const std::string& name);

    /**
     * @brief destructor
     */
    virtual ~Oh5Node();
    
    /**
     * @brief clone this node
     *
     * @note the cloned node is not associated with a backend
     */
    virtual Oh5Node* clone() const { return do_clone(); }

    bool has_backend() const { return backend_ != 0; }
    
    Oh5NodeBackend& backend();
    const Oh5NodeBackend& backend() const;
        
    /**
     * @brief access node name
     */
    std::string name() const { return name_; }
    
    /**
     * @brief absolute path of this node
     *
     * path is the concatenation of all node names leading from root to
     * this node, separated by "/".
     */
    std::string path() const;
    
    /**
     * @brief add a child node
     * @param node the node to add
     * @pre this node must be associated with a backend
     */
    Oh5Node& add(Oh5Node* node);

    /**
     * @brief parent node
     * @return pointer to a parent or null pointer if this node has no parent
     */
    Oh5Node* parent() { return parent_; }
    
    /**
     * @copydoc parent()
     */
    const Oh5Node* parent() const { return parent_; }
    
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
    Oh5Node* child(const std::string& path);
    
    /**
     * @copydoc child()
     */
    const Oh5Node* child(const std::string& path) const;

    /**
     * @brief child node of type T
     * @tparam T type to test parent against
     * @param path path to the child, relative to this node
     * @return pointer to a child node of type T or null pointer if not found
     */
    template<typename T>
    T* child(const std::string& path) {
        return dynamic_cast<T*>(child(path));
    }

    /**
     * @brief child node of type T
     * @tparam T type to test parent against
     * @param path path to the child, relative to this node
     * @return pointer to a child node of type T or null pointer if not found
     */
    template<typename T>
    const T* child(const std::string& path) const {
        return dynamic_cast<const T*>(child(path));
    }

    /**
     * @brief test if this node can have @c node as a child
     * @sa do_accepts_child
     */
    bool accepts_child(const Oh5Node& node) const;
    
    /**
     * @brief access children
     *
     * forwards to NodeBackend::children()
     */
    std::vector<const Oh5Node*> children() const;
    
    /**
     * @brief access children
     *
     * forwards to NodeBackend::children() const
     */
    std::vector<Oh5Node*> children();

    /**
     * @brief access a child attribute
     * @param path path to the attribute
     * @return pointer to Attribute or null if not found
     * @sa child()
     */
    Oh5Attribute* attribute(const std::string& name);
    
    /**
     * @copydoc attribute()
     */
    const Oh5Attribute* attribute(const std::string& name) const;

    /**
     * @brief access child group
     * @param path path to the group
     * @return pointer to Group or null if not found
     * @sa Node::child()
     */
    Oh5Group* group(const std::string& path);
    
    /**
     * @copydoc group()
     */
    const Oh5Group* group(const std::string& path) const;
    
    iterator begin();
    const_iterator begin() const;
    
    iterator end();
    const_iterator end() const;

    /**
     * @brief associate with backend
     * @throw runtime_error when already associated with a backend
     */
    void backend(Oh5NodeBackend* backend);
    
    /**
     * @brief set parent
     * @throw runtime_error when already associated with a parent
     */
    void parent(Oh5Node* parent);
  
  protected:
    Oh5Node(const Oh5Node& other);

  private:
    virtual bool do_accepts_child(const Oh5Node& node) const = 0;

    virtual Oh5Node* do_clone() const = 0;

    std::string name_;
    Oh5NodeBackend* backend_;
    Oh5Node* parent_;
};

template<typename T>
class Oh5NodeIterator :
        public boost::iterator_facade<Oh5NodeIterator<T>,
                                      T,
                                      boost::forward_traversal_tag> {
  public:
    explicit Oh5NodeIterator(T* node=0);

    template<typename OtherT>
    Oh5NodeIterator(const Oh5NodeIterator<OtherT>& other);

  private:
    friend class boost::iterator_core_access;
    template<typename>
    friend class Oh5NodeIterator;
    
    void increment();

    T& dereference() const;

    template<typename OtherT>
    bool equal(const Oh5NodeIterator<OtherT>& rhs) const;

    std::deque<T*> stack_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_HPP
