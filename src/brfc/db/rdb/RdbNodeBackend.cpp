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

#include <brfc/db/rdb/RdbNodeBackend.hpp>

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

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>
#include <brfc/oh5/Group.hpp>

namespace mi = boost::multi_index;

namespace brfc {
namespace db {
namespace rdb {

struct RdbNodeBackend::Impl {
    struct node_entry {
        explicit node_entry(const oh5::Node* node_,
                            long long id_=0,
                            bool loaded_=true)
                : node(node_)
                , id(id_)
                , loaded(loaded_) {

        }

        std::string name() const { return node->name(); }

        const oh5::Node* parent() const { return node->parent(); }

        const oh5::Node* node;
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
                    node_entry, const oh5::Node*, &node_entry::node>
            >,
            mi::hashed_non_unique<
                mi::tag<by_parent>,
                mi::const_mem_fun<
                    node_entry, const oh5::Node*, &node_entry::parent>
            >,
            mi::hashed_unique<
                mi::composite_key<
                    node_entry,
                    mi::const_mem_fun<node_entry, const oh5::Node*, &node_entry::parent>,
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
        cmp_ptr(const oh5::Node* n) : n_(n) { }

        bool operator()(const oh5::Node& n) { return n_ == &n; }

        const oh5::Node* n_;
    };

    Impl(RelationalDatabase* rdb_)
            : rdb(rdb_)
            , nodes()
            , entries() {
        nodes.push_back(new oh5::Group(""));
        entries.insert(node_entry(&root(), 0));
    }

    ~Impl() {

    }

    bool loaded(const oh5::Node& node) const {
        return entry(node).loaded;
    }

    void loaded(const oh5::Node& node, bool loaded) {
        entries.get<by_node>().modify(find_entry(node), set_loaded(loaded));
    }

    long long id(const oh5::Node& node) const {
        return entry(node).id;
    }

    void id(const oh5::Node& node, long long id) {
        entries.get<by_node>().modify(find_entry(node), set_id(id));
    }

    node_entry entry(const oh5::Node& node) const {
        const EntryByNode_t& nentries = entries.get<by_node>();
        EntryByNode_t::const_iterator iter = nentries.find(&node);
        if (iter == nentries.end())
            throw lookup_error("couldn't find node");
        return *iter;
    }

    EntryByNode_t::iterator
    find_entry(const oh5::Node& node) {
        return entries.get<by_node>().find(&node);
    }

    EntryByNode_t::const_iterator
    find_entry(const oh5::Node& node) const {
        return entries.get<by_node>().find(&node);
    }

    void add(oh5::Node* _node) {
        auto_ptr<oh5::Node> node(_node);
        if (not entries.insert(node_entry(_node)).second)
            throw duplicate_entry(node->path());
        nodes.push_back(node);
    }

    bool has(const oh5::Node& node) const {
        return std::find_if(nodes.begin(), nodes.end(),
                            cmp_ptr(&node)) != nodes.end();
    }

    void load(const oh5::Node& node) const {
        // XXX: !
        Impl* self = const_cast<Impl*>(this);
        EntryByNode_t& es = self->entries.get<by_node>();
        EntryByNode_t::iterator iter = es.find(&node);
        BRFC_ASSERT(iter != es.end());

        if (iter->loaded)
            return;
        
        RdbHelper helper(rdb->conn());
        // XXX: !
        oh5::Node& n = const_cast<oh5::Node&>(node);
        helper.load_children(n);
        
        es.modify(iter, set_loaded(true));
    }

    oh5::Node& root() { return nodes.front(); }

    const oh5::Node& root() const { return nodes.front(); }

    std::vector<const oh5::Node*> children(const oh5::Node& node) const {
        load(node);

        const EntryByParent_t& nentries = entries.get<by_parent>();

        std::pair<EntryByParent_t::iterator,
                  EntryByParent_t::iterator> range;
        range = nentries.equal_range(&node);
        
        std::vector<const oh5::Node*> vec;
        for ( ; range.first != range.second; ++range.first) {
            vec.push_back(range.first->node);
        }
        return vec;
    }
     
    RelationalDatabase* rdb;
    boost::ptr_vector<oh5::Node> nodes;
    NodeSet_t entries;
};

RdbNodeBackend::RdbNodeBackend(RelationalDatabase* rdb)
        : impl_(new Impl(rdb)) {
    impl_->root().backend(this);
}

RdbNodeBackend::~RdbNodeBackend() {

}

void
RdbNodeBackend::id(const oh5::Node& node, long long id) {
    impl_->id(node, id);
}

long long
RdbNodeBackend::id(const oh5::Node& node) const {
    return impl_->id(node);
}

void
RdbNodeBackend::loaded(const oh5::Node& node, bool loaded) {
    return impl_->loaded(node, loaded);
}

bool
RdbNodeBackend::loaded(const oh5::Node& node) const {
    return impl_->loaded(node);
}

oh5::Node&
RdbNodeBackend::do_add(oh5::Node* node) {
    impl_->add(node);
    return *node;
}

bool
RdbNodeBackend::do_has(const oh5::Node& node) const {
    return impl_->has(node);
}

const oh5::Node&
RdbNodeBackend::do_root() const {
    return impl_->root();
}

std::vector<const oh5::Node*>
RdbNodeBackend::do_children(const oh5::Node& node) const {
    return impl_->children(node);
}

} // namespace rdb
} // namespace db
} // namespace brfc
