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

#include <memory>
#include <string>

#include <brfc/db/rdb/RdbFileEntry.hpp>

#include <brfc/assert.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/sql/Connection.hpp>

namespace brfc {
namespace db {
namespace rdb {

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
        , root_(this) {
    BRFC_ASSERT(rdb_ != 0);
    auto_ptr<RdbNodeBackend> be(new RdbNodeBackend());
    be->loaded(false);
    root_.backend(be.release());
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
    RdbHelper helper(rdb().conn());
    try {
        helper.load_file(*self);
    } catch (...) {
        self->loaded(false);
        throw;
    }
    helper.backend(self->root()).id(helper.select_root_id(*this));
}

const oh5::Group&
RdbFileEntry::do_root() const {
    if ((id_ or not uuid_.empty()) and not loaded())
        load();
    return root_;
}

oh5::Source
RdbFileEntry::do_source() const {
    RdbHelper helper(rdb().conn());
    if (source_.empty()) {
        RdbFileEntry* self = const_cast<RdbFileEntry*>(this);
        self->source_ = helper.select_source(source_id());
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
    if (not loaded())
        load();
    return stored_at_;
}

void
RdbFileEntry::do_write_to_file(const std::string& path) const {
    shared_ptr<sql::Connection> conn = rdb().conn();
    conn->begin();
    try {
        conn->large_object_to_file(lo_id_, path);
        conn->commit();
    } catch (...) {
        conn->rollback();
        throw;
    }
}

} // namespace rdb
} // namespace db
} // namespace brfc
