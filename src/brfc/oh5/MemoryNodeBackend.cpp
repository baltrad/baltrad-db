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

#include <brfc/oh5/MemoryNodeBackend.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

namespace mi = boost::multi_index;

namespace brfc {
namespace oh5 {

struct MemoryNodeBackend::Impl {
    struct node_entry {
        node_entry(const Node* node_,
                   const Node* parent_)
                : node(node_)
                , parent(parent_) {

        }

        std::string name() const { return node->name(); }

        const Node* node;
        const Node* parent;
    };

    struct by_node {};
    struct by_parent {};

    typedef boost::multi_index_container<
        node_entry,
        mi::indexed_by<
            mi::hashed_unique<
                mi::tag<by_node>,
                mi::member<node_entry, const Node*, &node_entry::node>
            >,
            mi::hashed_non_unique<
                mi::tag<by_parent>,
                mi::member<node_entry, const Node*, &node_entry::parent>
            >,
            mi::hashed_unique<
                mi::composite_key<
                    node_entry,
                    mi::member<node_entry, const Node*, &node_entry::parent>,
                    mi::const_mem_fun<node_entry, std::string, &node_entry::name>
                >
            >
        >
    > NodeSet_t;

    typedef NodeSet_t::index<by_node>::type EntryByNode_t;
    typedef NodeSet_t::index<by_parent>::type EntryByParent_t;

    struct cmp_ptr {
        cmp_ptr(const Node* n) : n_(n) { }

        bool operator()(const Node& n) { return n_ == &n; }

        const Node* n_;
    };

    Impl()
            : nodes()
            , entries() {
        nodes.push_back(new Group(""));
        entries.insert(node_entry(&root(), 0));
    }

    ~Impl() {

    }

    void add(const Node& parent, oh5::Node* _node) {
        std::auto_ptr<Node> node(_node);
        if (not entries.insert(node_entry(_node, &parent)).second)
            throw duplicate_entry(parent.path() + "/" + node->name());
        nodes.push_back(node);
    }

    bool has(const Node& node) const {
        return std::find_if(nodes.begin(), nodes.end(),
                            cmp_ptr(&node)) != nodes.end();
    }

    Node& root() { return nodes.front(); }

    const Node& root() const { return nodes.front(); }

    const Node* parent(const Node& node) const {
        const Node* p = 0;
        const EntryByNode_t& nentries = entries.get<by_node>();
        EntryByNode_t::const_iterator iter = nentries.find(&node);
        if (iter != nentries.end())
            p = iter->parent;
        return p;
    }

    std::vector<const Node*> children(const oh5::Node& node) const {
        const EntryByParent_t& nentries = entries.get<by_parent>();

        std::pair<EntryByParent_t::iterator,
                  EntryByParent_t::iterator> range;
        range = nentries.equal_range(&node);
        
        std::vector<const Node*> vec;
        for ( ; range.first != range.second; ++range.first) {
            vec.push_back(range.first->node);
        }
        return vec;
    }

    boost::ptr_vector<Node> nodes;
    NodeSet_t entries;
};

MemoryNodeBackend::MemoryNodeBackend()
        : impl_(new Impl()) {
    root(impl_->root());
}

MemoryNodeBackend::~MemoryNodeBackend() {

}

Node&
MemoryNodeBackend::do_add(const Node& parent, oh5::Node* node) {
    impl_->add(parent, node);
    return *node;
}

bool
MemoryNodeBackend::do_has(const Node& node) const {
    return impl_->has(node);
}

const Node&
MemoryNodeBackend::do_root() const {
    return impl_->root();
}

const Node*
MemoryNodeBackend::do_parent(const Node& node) const {
    return impl_->parent(node);
}

std::vector<const Node*>
MemoryNodeBackend::do_children(const Node& node) const {
    return impl_->children(node);
}

} // namespace oh5
} // namespace brfc

