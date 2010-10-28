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

#include <brfc/db/rdb/RdbFileEntry.hpp>

#include <brfc/assert.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/String.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/LargeObject.hpp>

namespace brfc {
namespace db {
namespace rdb {

RdbFileEntry::RdbFileEntry(RelationalDatabase* rdb, long long id)
        : rdb_(rdb)
        , id_(id)
        , lo_id_(0)
        , source_id_(0)
        , source_()
        , uuid_()
        , hash_()
        , root_(this) {
    BRFC_ASSERT(rdb_ != 0);
    auto_ptr<RdbNodeBackend> root_backend(new RdbNodeBackend());
    if (id != 0)
        root_backend->id(rdb->helper().select_root_id(*this));
    root_.backend(root_backend.release());
}

RdbFileEntry::~RdbFileEntry() {

}

String
RdbFileEntry::do_uuid() const {
    if (id_ != 0 and uuid_ == "")
        load();
    return uuid_;
}

long long
RdbFileEntry::lo_id() const {
    if (lo_id_ == 0)
        load();
    return lo_id_;
}

long long
RdbFileEntry::source_id() const {
    if (source_id_ == 0)
        load();
    return source_id_;
}

void
RdbFileEntry::load() const {
    RdbFileEntry* self = const_cast<RdbFileEntry*>(this);
    rdb().helper().load_file(*self);
}

oh5::Source
RdbFileEntry::do_source() const {
    if (source_.empty()) {
        RdbFileEntry* self = const_cast<RdbFileEntry*>(this);
        self->source_ = rdb().helper().select_source(source_id());
    }
    return source_;
}

String
RdbFileEntry::do_hash() const {
    if (id_ != 0 and hash_ == "") {
        load();
    }
    return hash_;
}

DateTime
RdbFileEntry::do_stored_at() const {
    if (id_ != 0 and stored_at_ == DateTime()) {
        load();
    }
    return stored_at_;
}

void
RdbFileEntry::do_write_to_file(const String& path) const {
    sql::Connection& conn = rdb().conn();
    conn.begin();
    try {
        conn.large_object(lo_id_)->write_to_file(path);
        conn.commit();
    } catch (...) {
        conn.rollback();
        throw;
    }
}

} // namespace rdb
} // namespace db
} // namespace brfc
