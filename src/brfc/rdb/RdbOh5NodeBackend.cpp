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

#include <brfc/rdb/RdbOh5NodeBackend.hpp>

#include <memory>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <brfc/rdb/RdbHelper.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/oh5/Oh5Group.hpp>

namespace mi = boost::multi_index;

namespace brfc {

struct RdbOh5NodeBackend::Impl {
    struct node_entry {
        explicit node_entry(const Oh5Node* node_,
                            long long id_=0,
                            bool loaded_=true)
                : node(node_)
                , id(id_)
                , loaded(loaded_) {

        }

        std::string name() const { return node->name(); }

        const Oh5Node* parent() const { return node->parent(); }

        const Oh5Node* node;
        long long id;
        bool loaded;
    };

    struct by_node {};
    struct by_parent {};

    typedef boost::multi_index_container<
        node_entry,
        mi::indexed_by<
            mi::hashed_unique<
                mi::tag<by_node>,
                mi::member<
                    node_entry, const Oh5Node*, &node_entry::node>
            >,
            mi::hashed_non_unique<
                mi::tag<by_parent>,
                mi::const_mem_fun<
                    node_entry, const Oh5Node*, &node_entry::parent>
            >,
            mi::hashed_unique<
                mi::composite_key<
                    node_entry,
                    mi::const_mem_fun<node_entry, const Oh5Node*, &node_entry::parent>,
                    mi::const_mem_fun<node_entry, std::string, &node_entry::name>
                >
            >
        >
    > NodeSet_t;

    typedef NodeSet_t::index<by_node>::type EntryByNode_t;
    typedef NodeSet_t::index<by_parent>::type EntryByParent_t;

    struct set_loaded {
        set_loaded(bool l) : l_(l) { }

        void operator()(node_entry& e) const { e.loaded = l_; }

        bool l_;
    };

    struct set_id {
        set_id(long long id) : id_(id) { }

        void operator()(node_entry& e) const { e.id = id_; }

        long long id_;
    };
    
    struct cmp_ptr {
        cmp_ptr(const Oh5Node* n) : n_(n) { }

        bool operator()(const Oh5Node& n) { return n_ == &n; }

        const Oh5Node* n_;
    };

    Impl(RelationalDatabase* rdb_)
            : rdb(rdb_)
            , nodes()
            , entries() {
        nodes.push_back(new Oh5Group(""));
        entries.insert(node_entry(&root(), 0));
    }

    ~Impl() {

    }

    bool loaded(const Oh5Node& node) const {
        return entry(node).loaded;
    }

    void loaded(const Oh5Node& node, bool loaded) {
        entries.get<by_node>().modify(find_entry(node), set_loaded(loaded));
    }

    long long id(const Oh5Node& node) const {
        return entry(node).id;
    }

    void id(const Oh5Node& node, long long id) {
        entries.get<by_node>().modify(find_entry(node), set_id(id));
    }

    node_entry entry(const Oh5Node& node) const {
        const EntryByNode_t& nentries = entries.get<by_node>();
        EntryByNode_t::const_iterator iter = nentries.find(&node);
        if (iter == nentries.end())
            throw lookup_error("couldn't find node");
        return *iter;
    }

    EntryByNode_t::iterator
    find_entry(const Oh5Node& node) {
        return entries.get<by_node>().find(&node);
    }

    EntryByNode_t::const_iterator
    find_entry(const Oh5Node& node) const {
        return entries.get<by_node>().find(&node);
    }

    void add(Oh5Node* _node) {
        std::auto_ptr<Oh5Node> node(_node);
        if (not entries.insert(node_entry(_node)).second)
            throw duplicate_entry(node->path());
        nodes.push_back(node);
    }

    bool has(const Oh5Node& node) const {
        return std::find_if(nodes.begin(), nodes.end(),
                            cmp_ptr(&node)) != nodes.end();
    }

    void load(const Oh5Node& node) const {
        // XXX: !
        Impl* self = const_cast<Impl*>(this);
        EntryByNode_t& es = self->entries.get<by_node>();
        EntryByNode_t::iterator iter = es.find(&node);
        BRFC_ASSERT(iter != es.end());

        if (iter->loaded)
            return;
        
        RdbHelper helper(rdb->conn());
        // XXX: !
        Oh5Node& n = const_cast<Oh5Node&>(node);
        helper.load_children(n);
        
        es.modify(iter, set_loaded(true));
    }

    Oh5Node& root() { return nodes.front(); }

    const Oh5Node& root() const { return nodes.front(); }

    std::vector<const Oh5Node*> children(const Oh5Node& node) const {
        load(node);

        const EntryByParent_t& nentries = entries.get<by_parent>();

        std::pair<EntryByParent_t::iterator,
                  EntryByParent_t::iterator> range;
        range = nentries.equal_range(&node);
        
        std::vector<const Oh5Node*> vec;
        for ( ; range.first != range.second; ++range.first) {
            vec.push_back(range.first->node);
        }
        return vec;
    }
     
    RelationalDatabase* rdb;
    boost::ptr_vector<Oh5Node> nodes;
    NodeSet_t entries;
};

RdbOh5NodeBackend::RdbOh5NodeBackend(RelationalDatabase* rdb)
        : impl_(new Impl(rdb)) {
    impl_->root().backend(this);
}

RdbOh5NodeBackend::~RdbOh5NodeBackend() {

}

void
RdbOh5NodeBackend::id(const Oh5Node& node, long long id) {
    impl_->id(node, id);
}

long long
RdbOh5NodeBackend::id(const Oh5Node& node) const {
    return impl_->id(node);
}

void
RdbOh5NodeBackend::loaded(const Oh5Node& node, bool loaded) {
    return impl_->loaded(node, loaded);
}

bool
RdbOh5NodeBackend::loaded(const Oh5Node& node) const {
    return impl_->loaded(node);
}

Oh5Node&
RdbOh5NodeBackend::do_add(Oh5Node* node) {
    impl_->add(node);
    return *node;
}

bool
RdbOh5NodeBackend::do_has(const Oh5Node& node) const {
    return impl_->has(node);
}

const Oh5Node&
RdbOh5NodeBackend::do_root() const {
    return impl_->root();
}

std::vector<const Oh5Node*>
RdbOh5NodeBackend::do_children(const Oh5Node& node) const {
    return impl_->children(node);
}

} // namespace brfc
