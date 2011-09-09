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

#include <string>

#include <brfc/rdb/RdbFileEntry.hpp>

#include <brfc/assert.hpp>

#include <brfc/rdb/RdbQuery.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

#include <brfc/sql/Connection.hpp>

namespace brfc {

RdbFileEntry::RdbFileEntry(RelationalDatabase* rdb)
        : rdb_(rdb)
        , loaded_(false)
        , id_(0)
        , lo_id_(0)
        , source_id_(0)
        , source_()
        , uuid_()
        , hash_()
        , size_(0)
        , stored_at_()
        , nodes_() {
    BRFC_ASSERT(rdb_ != 0);
}

RdbFileEntry::~RdbFileEntry() {

}

std::string
RdbFileEntry::do_uuid() const {
    if (uuid_.empty() and not loaded())
        load();
    return uuid_;
}

long long
RdbFileEntry::lo_id() const {
    if (lo_id_ == 0 and not loaded())
        load();
    return lo_id_;
}

long long
RdbFileEntry::source_id() const {
    if (source_id_ == 0 and not loaded())
        load();
    return source_id_;
}

void
RdbFileEntry::load() const {
    RdbFileEntry* self = const_cast<RdbFileEntry*>(this);
    self->loaded(true); // to disable recursion
    RdbQuery query(rdb().conn());
    try {
        query.load_file(*self);
    } catch (...) {
        self->loaded(false);
        throw;
    }
    query.load_nodes(id(), self->root());
}

const Oh5Node&
RdbFileEntry::do_root() const {
    if ((id_ or not uuid_.empty()) and not loaded())
        load();
    return nodes_.root();
}

Oh5Source
RdbFileEntry::do_source() const {
    RdbQuery query(rdb().conn());
    if (source_.empty()) {
        RdbFileEntry* self = const_cast<RdbFileEntry*>(this);
        self->source_ = query.select_source(source_id());
    }
    return source_;
}

std::string
RdbFileEntry::do_hash() const {
    if (hash_.empty() and not loaded())
        load();
    return hash_;
}

long long
RdbFileEntry::do_size() const {
    if (size_ == 0 and not loaded())
        load();
    return size_;
}

DateTime
RdbFileEntry::do_stored_at() const {
    if (stored_at_ == DateTime() and not loaded())
        load();
    return stored_at_;
}

void
RdbFileEntry::do_write_to_file(const std::string& path) const {
    rdb().entry_to_file(*this, path);
}

} // namespace brfc
