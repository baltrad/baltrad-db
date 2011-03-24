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

#include <brfc/oh5/Node.hpp>

#include <memory>
#include <list>

#include <boost/foreach.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/NodeBackend.hpp>

namespace brfc {
namespace oh5 {

Node::Node(const std::string& name)
        : boost::noncopyable()
        , name_(name)
        , backend_(0)
        , parent_(0) {
    if (name.find("/") != std::string::npos)
        throw value_error("invalid node name: " + name);
}

Node::Node(const Node& other)
        : name_(other.name_)
        , backend_(0)
        , parent_(0) {

}

Node::~Node() {

}

NodeBackend&
Node::backend() {
    if (not has_backend())
        throw std::runtime_error("no backend associated with node");
    return *backend_;
}

const NodeBackend&
Node::backend() const {
    if (not has_backend())
        throw std::runtime_error("no backend associated with node");
    return *backend_;
}

void
Node::backend(NodeBackend* backend) {
    if (has_backend())
        throw std::runtime_error("can't change backend of already bound node");
    backend_ = backend;
}

std::string
Node::path() const {
    std::list<std::string> names;
    const Node* nodep = this;
    while (nodep != 0) {
        names.push_front(nodep->name());
        nodep = nodep->parent();
    }

    std::string path = boost::join(names, "/");
    if (not boost::starts_with(path, "/"))
        path = "/" + path;
    return path;
}

Node&
Node::add(Node* node) {
    std::auto_ptr<Node> nodep(node);
    NodeBackend& be = backend();
    return be.add(*this, nodep.release());
}

bool
Node::has_child(const std::string& path) const {
    return child(path) != 0;
}

Node*
Node::child(const std::string& path) {
    const Node* self = const_cast<const Node*>(this);
    return const_cast<Node*>(self->child(path));
}

const Node*
Node::child(const std::string& path) const {
    return backend().child_by_path(*this, path);
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

bool
Node::accepts_child(const Node& node) const {
    return do_accepts_child(node);
}

std::vector<const Node*>
Node::children() const {
    return backend().children(*this);
}

std::vector<Node*>
Node::children() {
    return backend().children(*this);
}

Attribute*
Node::attribute(const std::string& path) {
    return child<Attribute>(path);
}

const Attribute*
Node::attribute(const std::string& path) const {
    return child<Attribute>(path);
}

Group*
Node::group(const std::string& path) {
    return child<Group>(path);
}

const Group*
Node::group(const std::string& path) const {
    return child<Group>(path);
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

// explicitly instantitate the templates
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
