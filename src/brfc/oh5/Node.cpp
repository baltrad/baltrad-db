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

#include <brfc/oh5/Node.hpp>

#include <algorithm>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

Node::Node(Node* parent, const String& name)
        : boost::noncopyable()
        , parent_(parent)
        , name_(name)
        , children_() {
    if (name.contains("/"))
        throw value_error("invalid node name: " + name.to_std_string());
}

Node::~Node() {

}

String
Node::path() const {
    StringList names;
    const Node* node = this;
    while (node != 0) {
        names.push_front(node->name());
        node = node->parent();
    }

    // ensure we get a / at the beginning
    if (names.front() != "")
        names.push_front("");

    return names.join("/");
}

Attribute&
Node::create_child_attribute(const String& name, const Scalar& value) {
    auto_ptr<Node> node(new Attribute(this, name, value));
    return static_cast<Attribute&>(add_child(node));
}

Group&
Node::create_child_group(const String& name) {
    auto_ptr<Node> node(Group::create_by_name(name));
    return static_cast<Group&>(add_child(node));
}

Node&
Node::add_child(auto_ptr<Node> node) {
    if (node.get() == 0)
        throw value_error("null pointer");
    if (has_child_by_name(node->name()))
        throw duplicate_entry(node->name().to_utf8());
    if (not accepts_child(*node))
        throw value_error("node '" + node->name().to_utf8() + "' not accepted as child of '" + name().to_utf8() + "'");
    if (not node->accepts_parent(*this))
        throw value_error("node does not accept this as parent");

    node->parent(this);
    children_.push_back(node);
    return children_.back();
}

bool
Node::has_child(const Node& node) const {
    if (node.parent() != this)
        return false;
    return has_child_by_name(node.name());
}

bool
Node::has_child_by_name(const String& name) const {
    BOOST_FOREACH(const Node& node, children_) {
        if (node.name() == name) {
            return true;
        }
    }
    return false;
}

Node*
Node::child_by_name(const String& name) {
    const Node* self = const_cast<const Node*>(this);
    return const_cast<Node*>(self->child_by_name(name));
}

const Node*
Node::child_by_name(const String& name) const {
    BOOST_FOREACH(const Node& node, children_) {
        if (node.name() == name) {
            return &node;
        }
    }
    return 0;
}

Node*
Node::child_by_path(const String& path) {
    const Node* self = const_cast<const Node*>(this);
    return const_cast<Node*>(self->child_by_path(path));
}

const Node*
Node::child_by_path(const String& path) const {
    StringList names = path.split("/");
    const Node* cur = this;
    BOOST_FOREACH(const String& name, names) {
        cur = cur->child_by_name(name);
        if (not cur)
            break;
    }
    return cur;
}

Node::iterator
Node::begin() {
    return iterator(this);
}

Node::const_iterator
Node::begin() const {
    return const_iterator(this);
}

Node::iterator
Node::end() {
    return iterator();
}

Node::const_iterator
Node::end() const {
    return const_iterator();
}

const Node&
Node::root() const {
    const Node* root = this;
    while (root->parent()) {
        root = root->parent();
    }
    return *root;
}

Node&
Node::root() {
    const Node* self = const_cast<const Node*>(this);
    return const_cast<Node&>(self->root());
}

std::vector<const Node*>
Node::children() const {
    std::vector<const Node*> vec;
    BOOST_FOREACH(const Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

std::vector<Node*>
Node::children() {
    std::vector<Node*> vec;
    BOOST_FOREACH(Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

const File*
Node::do_file() const {
    if (is_root()) {
        return 0;
    } else {
        return root().file();
    }
}

/***
 * NodeIterator implementation
 */

template<typename T>
NodeIterator<T>::NodeIterator(T* node)
        : stack_() {
    if (node != 0)
        stack_.push_back(node);
}

template<typename T>
template<typename OtherT>
NodeIterator<T>::NodeIterator(const NodeIterator<OtherT>& other)
        : stack_(other.stack_.begin(), other.stack_.end()) {

}

template<typename T>
void
NodeIterator<T>::increment() {
    if (not stack_.empty()) {
        T* cur = stack_.front();
        stack_.pop_front();
        BOOST_FOREACH(T* child, cur->children()) {
            stack_.push_back(child);
        }
    }
}

template<typename T>
T&
NodeIterator<T>::dereference() const {
    BRFC_ASSERT(not stack_.empty());
    return *stack_.front();
}

template<typename T>
template<typename OtherT>
bool
NodeIterator<T>::equal(const NodeIterator<OtherT>& rhs) const {
    if (stack_.empty() && rhs.stack_.empty()) {
        return true;
    } else if (not stack_.empty() && not rhs.stack_.empty()) {
        return stack_.front() == rhs.stack_.front();
    } else {
        return false;
    }
}

// explicitly instatitate the templates
template class NodeIterator<Node>;
template class NodeIterator<const Node>;

template
bool NodeIterator<Node>::equal(const NodeIterator<const Node>&) const;

template
bool NodeIterator<Node>::equal(const NodeIterator<Node>&) const;

template
bool NodeIterator<const Node>::equal(const NodeIterator<Node>&) const;

template
bool NodeIterator<const Node>::equal(const NodeIterator<const Node>&) const;


} // namespace oh5
} // namespace brfc
