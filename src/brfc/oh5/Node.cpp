#include <brfc/oh5/Node.hpp>

#include <algorithm>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

namespace brfc {
namespace oh5 {

Node::Node(const QString& name)
        : boost::noncopyable()
        , enable_shared_from_this<Node>()
        , name_(name)
        , parent_()
        , children_() {
    BRFC_ASSERT(not name.contains("/"));
}

Node::~Node() {

}

void
Node::name(const QString& name) {
    shared_ptr<Node> p = parent_.lock();
    if (p and p->has_child_by_name(name)) {
       throw duplicate_entry("changing name results in duplicate child");
    }
    name_ = name;
}

QString
Node::path() const {
    QStringList names;
    const Node* node = this;
    while (node != 0) {
        names.push_front(node->name());
        node = node->parent().get();
    }

    // ensure we get a / at the beginning
    if (names.front() != "")
        names.push_front("");

    return names.join("/");
}

void
Node::parent(shared_ptr<Node> node) {
    parent_ = node;
}

void
Node::add_child(shared_ptr<Node> node) {
    if (not node) {
        throw value_error("null pointer");
    } else if (has_child_by_name(node->name())) {
        throw duplicate_entry(node->name().toUtf8().constData());
    } else if (not accepts_child(*node)) {
        throw value_error("node not accepted as child");
    } else if (not node->accepts_parent(*this)) {
        throw value_error("node does not accept this as parent");
    }

    shared_ptr<Node> old_parent = node->parent();
    if (old_parent) {
        old_parent->remove_child(*node);
    }

    children_.push_back(node);
    node->parent(shared_from_this());
}

void
Node::remove_child(Node& node) {
    ChildVector::iterator iter = std::find(children_.begin(),
                                           children_.end(),
                                           node.shared_from_this());
    if (iter != children_.end()) {
        children_.erase(iter);
    } else {
        throw lookup_error("child not found");
    }

    node.parent(shared_ptr<Node>());
}

bool
Node::has_child(const Node& node) const {
    ChildVector::const_iterator iter = std::find(children_.begin(),
                                                 children_.end(),
                                                 node.shared_from_this());
    return iter != children_.end();
}

bool
Node::has_child_by_name(const QString& name) const {
    BOOST_FOREACH(shared_ptr<Node> node, children_) {
        if (node->name() == name) {
            return true;
        }
    }
    return false;
}

Node::iterator
Node::begin() {
    return iterator(*this);
}

Node::const_iterator
Node::begin() const {
    return const_iterator(*this);
}

Node::iterator
Node::end() {
    return iterator();
}

Node::const_iterator
Node::end() const {
    return const_iterator();
}


/***
 * NodeIterator implementation
 */

template<typename T>
NodeIterator<T>::NodeIterator()
        : stack_() {

}

template<typename T>
NodeIterator<T>::NodeIterator(T& node)
        : stack_() {
    stack_.push_back(node.shared_from_this());
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
        shared_ptr<T> cur = stack_.front();
        stack_.pop_front();
        BOOST_FOREACH(shared_ptr<T> child, cur->children_) {
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
