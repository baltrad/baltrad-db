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

#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <deque>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/String.hpp>
#include <brfc/SHA1AttributeHasher.hpp>
#include <brfc/StringList.hpp>

#include <brfc/db/FileQuery.hpp>
#include <brfc/db/FileResult.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>
#include <brfc/db/rdb/QueryToSelect.hpp>
#include <brfc/db/rdb/RdbAttributeResult.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbFileResult.hpp>
#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/SaveFile.hpp>

#include <brfc/oh5/PhysicalFile.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/sql/expr.hpp>
#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

namespace brfc {
namespace db {
namespace rdb {

RelationalDatabase::RelationalDatabase(const String& dsn_)
        : conn_(sql::Connection::create(dsn_))
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher())
        , helper_(new RdbHelper(conn_.get(), file_hasher_.get())) {
    conn_->open();
    populate_mapper();
    populate_hasher();
}

RelationalDatabase::RelationalDatabase(shared_ptr<sql::Connection> conn,
                                       shared_ptr<RdbHelper> helper)
        : conn_(conn)
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher())
        , helper_(helper) {
    BRFC_ASSERT(conn);
    populate_mapper();
    populate_hasher();
}

RelationalDatabase::~RelationalDatabase() {

}

AttributeMapper&
RelationalDatabase::mapper() {
    return *mapper_;
}

const AttributeMapper&
RelationalDatabase::mapper() const {
    return *mapper_;
}

bool
RelationalDatabase::do_is_stored(const oh5::PhysicalFile& file) {
    const Model& m = Model::instance();

    const String& hash = file_hasher().hash(file);
    long long src_id = helper().select_source_id(file.source());

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create();
    qry->what(xpr.bool_(true));
    qry->from(m.files);
    qry->where(
        xpr.and_(
            m.files->column("hash")->eq(xpr.string(hash)),
            m.files->column("source_id")->eq(xpr.int64_(src_id))
        )
    );

    shared_ptr<sql::Result> result = conn().execute(*qry);
    return result->size() > 0;
}

shared_ptr<FileEntry>
RelationalDatabase::do_store(const oh5::PhysicalFile& file) {
    shared_ptr<RdbFileEntry> entry(new RdbFileEntry(this));
    conn().begin();
    try {
        SaveFile save(*entry);
        save(file);
        conn().commit();
    } catch (const std::runtime_error&) {
        conn().rollback();
        throw;
    }
    return entry;
}

shared_ptr<FileEntry>
RelationalDatabase::do_entry_by_uuid(const String& uuid) {
    shared_ptr<RdbFileEntry> entry(new RdbFileEntry(this));
    entry->uuid(uuid);
    entry->load();
    return entry;
}

shared_ptr<FileResult>
RelationalDatabase::do_execute(const FileQuery& query) {
    sql::SelectPtr select = QueryToSelect(&mapper()).transform(query);
    shared_ptr<sql::Result> res = conn().execute(*select);
    return shared_ptr<FileResult>(new RdbFileResult(this, res));
}

shared_ptr<AttributeResult>
RelationalDatabase::do_execute(const AttributeQuery& query) {
    sql::SelectPtr select = QueryToSelect(&mapper()).transform(query);
    shared_ptr<sql::Result> res = conn().execute(*select);
    return shared_ptr<AttributeResult>(new RdbAttributeResult(res));
}

void
RelationalDatabase::populate_mapper() {
    const Model& m = Model::instance();

    mapper_->clear();
    mapper_->add(Mapping("file:uuid", m.files->column("uuid")));
    mapper_->add(Mapping("file:hash", m.files->column("hash")));
    mapper_->add(Mapping("file:stored_at", m.files->column("stored_at")));
    mapper_->add(Mapping("source:name", m.sources->column("name")));
    mapper_->add(Mapping("what/object", m.files->column("what_object")));
    mapper_->add(Mapping("what/date", m.files->column("what_date")));
    mapper_->add(Mapping("what/time", m.files->column("what_time")));
}

void
RelationalDatabase::populate_hasher() {
    file_hasher_->clear_ignored();
    file_hasher_->ignore("what/source");
}

bool
RelationalDatabase::do_remove(const FileEntry& entry) {
    String qry("DELETE FROM bdb_files WHERE uuid = :uuid");
    sql::BindMap binds;
    binds.add(":uuid", Variant(entry.uuid()));
    shared_ptr<sql::Result> r = conn().execute(qry, binds);
    return r->affected_rows();
}

} // namespace rdb
} // namespace db
} // namespace brfc
