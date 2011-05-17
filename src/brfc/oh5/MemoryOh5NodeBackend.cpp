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

#include <brfc/oh5/MemoryOh5NodeBackend.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>

namespace mi = boost::multi_index;

namespace brfc {
namespace oh5 {

struct MemoryOh5NodeBackend::Impl {

    struct by_node {};
    struct by_parent {};

    typedef boost::multi_index_container<
        const Oh5Node*,
        mi::indexed_by<
            mi::hashed_unique<
                mi::tag<by_node>,
                mi::identity<const Oh5Node*>
            >,
            mi::hashed_non_unique<
                mi::tag<by_parent>,
                mi::const_mem_fun<Oh5Node, const Oh5Node*, &Oh5Node::parent>
            >,
            mi::hashed_unique<
                mi::composite_key<
                    const Oh5Node*,
                    mi::const_mem_fun<Oh5Node, const Oh5Node*, &Oh5Node::parent>,
                    mi::const_mem_fun<Oh5Node, std::string, &Oh5Node::name>
                >
            >
        >
    > NodeSet_t;

    typedef NodeSet_t::index<by_node>::type EntryByNode_t;
    typedef NodeSet_t::index<by_parent>::type EntryByParent_t;

    struct cmp_ptr {
        cmp_ptr(const Oh5Node* n) : n_(n) { }

        bool operator()(const Oh5Node& n) { return n_ == &n; }

        const Oh5Node* n_;
    };

    Impl()
            : nodes()
            , entries() {
        nodes.push_back(new Oh5Group(""));
        entries.insert(&root());
    }

    ~Impl() {

    }

    void add(oh5::Oh5Node* _node) {
        std::auto_ptr<Oh5Node> node(_node);
        if (not entries.insert(_node).second)
            throw duplicate_entry(node->path());
        nodes.push_back(node);
    }

    bool has(const Oh5Node& node) const {
        return std::find_if(nodes.begin(), nodes.end(),
                            cmp_ptr(&node)) != nodes.end();
    }

    Oh5Node& root() { return nodes.front(); }

    const Oh5Node& root() const { return nodes.front(); }

    std::vector<const Oh5Node*> children(const oh5::Oh5Node& node) const {
        const EntryByParent_t& nentries = entries.get<by_parent>();

        std::pair<EntryByParent_t::iterator,
                  EntryByParent_t::iterator> range;
        range = nentries.equal_range(&node);
        
        std::vector<const Oh5Node*> vec;
        for ( ; range.first != range.second; ++range.first) {
            vec.push_back(*range.first);
        }
        return vec;
    }

    boost::ptr_vector<Oh5Node> nodes;
    NodeSet_t entries;
};

MemoryOh5NodeBackend::MemoryOh5NodeBackend()
        : impl_(new Impl()) {
    impl_->root().backend(this);
}

MemoryOh5NodeBackend::~MemoryOh5NodeBackend() {

}

Oh5Node&
MemoryOh5NodeBackend::do_add(oh5::Oh5Node* node) {
    impl_->add(node);
    return *node;
}

bool
MemoryOh5NodeBackend::do_has(const Oh5Node& node) const {
    return impl_->has(node);
}

const Oh5Node&
MemoryOh5NodeBackend::do_root() const {
    return impl_->root();
}

std::vector<const Oh5Node*>
MemoryOh5NodeBackend::do_children(const Oh5Node& node) const {
    return impl_->children(node);
}

} // namespace oh5
} // namespace brfc

