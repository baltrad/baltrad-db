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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_OH5_NODE_HPP
#define BRFC_OH5_NODE_HPP

#include <deque>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <QtCore/QString>

#include <brfc/smart_ptr.hpp>

namespace brfc {
namespace oh5 {

class File;

template<typename T>
class NodeIterator;

/**
 * @brief ABC for all ODIM_H5 elements
 *
 * A node is a single element in the ODIM_H5 hirerchy. It has a name which
 * is unique among the children of one parent.
 *
 * A node can be associated with one parent only, so reassociating a node is
 * logically equivalent to moving it.
 */
class Node : public boost::noncopyable,
             public enable_shared_from_this<Node> {
  public:
    typedef std::vector<shared_ptr<Node> > ChildVector;
    typedef NodeIterator<Node> iterator;
    typedef NodeIterator<const Node> const_iterator;

    /**
     * @brief destructor
     */
    virtual ~Node();

    /**
     * @{
     */
    const QString& name() const { return name_; }

    void name(const QString& name);
    ///@}
    
    /**
     * @brief absolute path of this node
     *
     * path is the concatenation of all node names leading from root to
     * this node, separated by "/".
     */
    QString path() const;

    /**
     * @brief parent node
     * @return pointer to a parent or null pointer if this node has no parent
     */
    shared_ptr<Node> parent() const {
        return parent_.lock();
    }

    /**
     * @brief is this node the root node
     */
    bool is_root() const {
        return parent_.expired();
    }
    
    /**
     * @{
     * @brief get root node
     */
    shared_ptr<Node> root();

    shared_ptr<const Node> root() const;
    ///@}

    /**
     * @brief add a child node
     * @throw duplicate_entry if a child with that name already exists
     * @throw value_error if child is not accepted or null
     *
     * First, a check for a null pointer is made, followed by a check for
     * duplicate name. Then this node has a chance to deny a child and
     * finally the child node has a change to deny the parent.
     *
     * If the child is previously associated with a parent node, it is
     * removed from that parents children.
     *
     * @sa accepts_child, accepts_parent
     */
    void add_child(shared_ptr<Node> child);

    /**
     * @brief test for a child by name
     */
    bool has_child_by_name(const QString& name) const;

    /**
     * @brief test for a child node
     */
    bool has_child(const Node& node) const;

    /**
     * @brief remove a child node
     * @throw lookup_error if child is not found
     *
     * on removing a node, its parent is set to null.
     */
    void remove_child(Node& node);
    
    /**
     * @{
     * @brief access child by name
     * @return pointer to Node or null if not found.
     */
    shared_ptr<Node>
    child_by_name(const QString& name);

    shared_ptr<const Node>
    child_by_name(const QString& name) const;
    ///@}
    
    /**
     * @{
     * @brief access child by path
     * @return pointer to Node or null if not found.
     */
    shared_ptr<Node>
    child_by_path(const QString& path);

    shared_ptr<const Node>
    child_by_path(const QString& path) const;
    ///@}

    /**
     * @sa do_accepts_child
     */
    bool accepts_child(const Node& node) const {
        return do_accepts_child(node);
    }

    /**
     * @sa do_accepts_parent
     */
    bool accepts_parent(const Node& node) const {
        return do_accepts_parent(node);
    }

    /**
     * @brief access children
     */
    const ChildVector& children() const {
        return children_;
    }

    /**
     * @brief file this node is associated with
     * @return pointer to File or null if not associated with a file
     *
     * default implementation returns the file associated with root and
     * for root returns a null pointer.
     * @{
     */
    shared_ptr<const File> file() const {
        return do_file();
    }

    shared_ptr<File> file() {
        const Node* self = const_cast<const Node*>(this);
        return const_pointer_cast<File>(self->file());
    }
    ///@}
    
    iterator begin();
    const_iterator begin() const;
    
    iterator end();
    const_iterator end() const;

  protected:
    // allow make_shared access to constructor
    /*
     * could use this for gcc >= 4.3 with -std=c++x0
    template<typename T, typename... Args> 
    friend shared_ptr<T> boost::make_shared(Args&& ...); 
    */

    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     *
     * use make_shared<Node> to call
     */
    Node(const QString& name);

    void parent(shared_ptr<Node> node);
    
    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual bool do_accepts_parent(const Node& node) const = 0;

    virtual shared_ptr<const File> do_file() const;

  private:
    friend class NodeIterator<Node>;
    friend class NodeIterator<const Node>;

    QString name_;
    weak_ptr<Node> parent_;
    ChildVector children_;

};

template<typename T>
class NodeIterator :
        public boost::iterator_facade<NodeIterator<T>,
                                      T,
                                      boost::forward_traversal_tag> {
  public:
    NodeIterator();

    explicit NodeIterator(T& node);

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

    std::deque<shared_ptr<T> > stack_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_HPP
