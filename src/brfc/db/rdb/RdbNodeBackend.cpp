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

#include <brfc/db/rdb/RdbNodeBackend.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace db {
namespace rdb {

RdbNodeBackend::RdbNodeBackend(long long id)
        : children_()
        , id_(id)
        , loaded_(true) {

}

RdbNodeBackend::~RdbNodeBackend() {

}

oh5::Node&
RdbNodeBackend::do_create_child(oh5::Node* _node) {
    auto_ptr<oh5::Node> node(_node);

    node->parent(this->front());
    node->backend(new RdbNodeBackend());

    children_.push_back(node);
    return children_.back();
}

RelationalDatabase&
RdbNodeBackend::rdb() const {
    RdbFileEntry* fe = static_cast<RdbFileEntry*>(front()->file());
    BRFC_ASSERT(fe);
    return fe->rdb();
}

void
RdbNodeBackend::load() const {
    RdbNodeBackend* self = const_cast<RdbNodeBackend*>(this);
    RdbHelper helper(rdb().conn());
    helper.load_children(*self->front());
    loaded_ = true;
}

std::vector<const oh5::Node*>
RdbNodeBackend::do_children() const {
    if (not loaded_)
        load();

    std::vector<const oh5::Node*> vec;
    BOOST_FOREACH(const oh5::Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

std::vector<oh5::Node*>
RdbNodeBackend::do_children() {
    if (not loaded_)
        load();

    std::vector<oh5::Node*> vec;
    BOOST_FOREACH(oh5::Node& node, children_) {
        vec.push_back(&node);
    }
    return vec;
}

} // namespace rdb
} // namespace db
} // namespace brfc
