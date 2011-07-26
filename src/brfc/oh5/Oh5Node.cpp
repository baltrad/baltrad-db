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

#include <brfc/oh5/Oh5Node.hpp>

#include <list>
#include <memory>

#include <boost/foreach.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5NodeBackend.hpp>

namespace brfc {

Oh5Node::Oh5Node(const std::string& name)
        : boost::noncopyable()
        , name_(name)
        , backend_(0)
        , parent_(0) {
    if (name.find("/") != std::string::npos)
        throw value_error("invalid node name: " + name);
}

Oh5Node::Oh5Node(const Oh5Node& other)
        : name_(other.name_)
        , backend_(0)
        , parent_(0) {

}

Oh5Node::~Oh5Node() {

}

Oh5NodeBackend&
Oh5Node::backend() {
    if (not has_backend())
        throw std::runtime_error("no backend associated with node");
    return *backend_;
}

const Oh5NodeBackend&
Oh5Node::backend() const {
    if (not has_backend())
        throw std::runtime_error("no backend associated with node");
    return *backend_;
}

void
Oh5Node::backend(Oh5NodeBackend* backend) {
    if (has_backend())
        throw std::runtime_error("can't change backend of already bound node");
    backend_ = backend;
}

void
Oh5Node::parent(Oh5Node* parent) {
    if (parent_ != 0)
        throw std::runtime_error("can't change parent of already bound node");
    parent_ = parent;
}

std::string
Oh5Node::path() const {
    std::list<std::string> names;
    const Oh5Node* nodep = this;
    while (nodep != 0) {
        names.push_front(nodep->name());
        nodep = nodep->parent();
    }

    std::string path = boost::join(names, "/");
    if (not boost::starts_with(path, "/"))
        path = "/" + path;
    return path;
}

Oh5Node&
Oh5Node::add(Oh5Node* node) {
    std::auto_ptr<Oh5Node> nodep(node);
    Oh5NodeBackend& be = backend();
    return be.add(*this, nodep.release());
}

bool
Oh5Node::has_child(const std::string& path) const {
    return child(path) != 0;
}

Oh5Node*
Oh5Node::child(const std::string& path) {
    const Oh5Node* self = const_cast<const Oh5Node*>(this);
    return const_cast<Oh5Node*>(self->child(path));
}

const Oh5Node*
Oh5Node::child(const std::string& path) const {
    return backend().child_by_path(*this, path);
}

Oh5Node::iterator
Oh5Node::begin() {
    return iterator(this);
}

Oh5Node::const_iterator
Oh5Node::begin() const {
    return const_iterator(this);
}

Oh5Node::iterator
Oh5Node::end() {
    return iterator();
}

Oh5Node::const_iterator
Oh5Node::end() const {
    return const_iterator();
}

bool
Oh5Node::accepts_child(const Oh5Node& node) const {
    return do_accepts_child(node);
}

std::vector<const Oh5Node*>
Oh5Node::children() const {
    return backend().children(*this);
}

std::vector<Oh5Node*>
Oh5Node::children() {
    return backend().children(*this);
}

Oh5Attribute*
Oh5Node::attribute(const std::string& path) {
    return child<Oh5Attribute>(path);
}

const Oh5Attribute*
Oh5Node::attribute(const std::string& path) const {
    return child<Oh5Attribute>(path);
}

Oh5Group*
Oh5Node::group(const std::string& path) {
    return child<Oh5Group>(path);
}

const Oh5Group*
Oh5Node::group(const std::string& path) const {
    return child<Oh5Group>(path);
}

/***
 * NodeIterator implementation
 */

template<typename T>
Oh5NodeIterator<T>::Oh5NodeIterator(T* node)
        : stack_() {
    if (node != 0)
        stack_.push_back(node);
}

template<typename T>
void
Oh5NodeIterator<T>::increment() {
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
Oh5NodeIterator<T>::dereference() const {
    BRFC_ASSERT(not stack_.empty());
    return *stack_.front();
}

template<typename T>
template<typename OtherT>
bool
Oh5NodeIterator<T>::equal(const Oh5NodeIterator<OtherT>& rhs) const {
    if (stack_.empty() && rhs.stack_.empty()) {
        return true;
    } else if (not stack_.empty() && not rhs.stack_.empty()) {
        return stack_.front() == rhs.stack_.front();
    } else {
        return false;
    }
}

// explicitly instantitate the templates
template class Oh5NodeIterator<Oh5Node>;
template class Oh5NodeIterator<const Oh5Node>;

template
bool Oh5NodeIterator<Oh5Node>::equal(const Oh5NodeIterator<const Oh5Node>&) const;

template
bool Oh5NodeIterator<Oh5Node>::equal(const Oh5NodeIterator<Oh5Node>&) const;

template
bool Oh5NodeIterator<const Oh5Node>::equal(const Oh5NodeIterator<Oh5Node>&) const;

template
bool Oh5NodeIterator<const Oh5Node>::equal(const Oh5NodeIterator<const Oh5Node>&) const;

} // namespace brfc
