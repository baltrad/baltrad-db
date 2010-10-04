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

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/NodeImpl.hpp>

namespace brfc {
namespace oh5 {

Node::Node(NodeImpl* impl)
        : boost::noncopyable()
        , impl_(impl) {
    if (impl_.get() == 0)
        throw value_error("null node impl");
    impl_->front(this);
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
Node::create_attribute(const String& name, const Scalar& value) {
    return impl().create_attribute(name, value);
}

Group&
Node::create_group(const String& name) {
    return impl().create_group(name);
}

const String&
Node::name() const {
    return impl().name();
}

Node*
Node::parent() {
    return impl().parent();
}

const Node*
Node::parent() const {
    return impl().parent();
}


bool
Node::has_child(const String& path) const {
    return child(path) != 0;
}

Node*
Node::child(const String& path) {
    const Node* self = const_cast<const Node*>(this);
    return const_cast<Node*>(self->child(path));
}

const Node*
Node::child(const String& path) const {
    if (path.starts_with("/") and not is_root())
        throw value_error("path must not be absolute");
    StringList names = path.split("/", String::SKIP_EMPTY_PARTS);
    const Node* cur = this;
    const Node* child = 0;
    BOOST_FOREACH(const String& name, names) {
        child = 0;
        BOOST_FOREACH(const Node* node, cur->children()) {
            if (node->name() == name) {
                child = node;
                break;
            }
        }
        if (not child)
            break;
        cur = child;
    }
    return child;
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
    return impl().children();
}

std::vector<Node*>
Node::children() {
    return impl().children();
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
