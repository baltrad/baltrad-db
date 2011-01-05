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
#include <string>
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
#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
#include <brfc/sql/Result.hpp>

namespace brfc {
namespace db {
namespace rdb {

RelationalDatabase::RelationalDatabase(const std::string& dsn)
        : creator_(new sql::DefaultConnectionCreator(dsn))
        , pool_(new sql::BasicConnectionPool(creator_))
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher()) {
    conn(); // check if connection is valid
    populate_mapper();
    populate_hasher();
}

RelationalDatabase::RelationalDatabase(shared_ptr<sql::ConnectionPool> pool)
        : creator_()
        , pool_(pool)
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher()) {
    BRFC_ASSERT(pool);
    populate_mapper();
    populate_hasher();
}

RelationalDatabase::~RelationalDatabase() {

}

shared_ptr<sql::Connection>
RelationalDatabase::conn() const {
    return pool_->get();
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
    try {
        entry_by_file(file);
    } catch (lookup_error) {
        return false;
    }
    return true;
}

FileEntry*
RelationalDatabase::do_store(const oh5::PhysicalFile& file) {
    return SaveFile(this)(file);
}

FileEntry*
RelationalDatabase::do_entry_by_file(const oh5::PhysicalFile& file) {
    const Model& m = Model::instance();

    const std::string& hash = file_hasher().hash(file);
    shared_ptr<sql::Connection> c = conn();
    long long src_id = RdbHelper(c).select_source_id(file.source());

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create();
    qry->what(m.files->column("uuid"));
    qry->from(m.files);
    qry->where(
        xpr.and_(
            m.files->column("hash")->eq(xpr.string(hash)),
            m.files->column("source_id")->eq(xpr.int64_(src_id))
        )
    );

    shared_ptr<sql::Result> result = c->execute(*qry);
    if (result->size() > 1 or not result->next())
        throw lookup_error(file.path() + " is not stored");
    
    return entry_by_uuid(result->value_at("uuid").string());
}

FileEntry*
RelationalDatabase::do_entry_by_uuid(const std::string& uuid) {
    auto_ptr<RdbFileEntry> entry(new RdbFileEntry(this));
    entry->uuid(uuid);
    entry->load();
    return entry.release();
}

FileResult*
RelationalDatabase::do_execute(const FileQuery& query) {
    sql::SelectPtr select = QueryToSelect(&mapper()).transform(query);
    shared_ptr<sql::Result> res = conn()->execute(*select);
    return new RdbFileResult(this, res);
}

AttributeResult*
RelationalDatabase::do_execute(const AttributeQuery& query) {
    sql::SelectPtr select = QueryToSelect(&mapper()).transform(query);
    shared_ptr<sql::Result> res = conn()->execute(*select);
    return new RdbAttributeResult(res);
}

void
RelationalDatabase::populate_mapper() {
    const Model& m = Model::instance();

    mapper_->clear();
    mapper_->add(Mapping("file:uuid", m.files->column("uuid")));
    mapper_->add(Mapping("file:hash", m.files->column("hash")));
    mapper_->add(Mapping("file:stored_at", m.files->column("stored_at")));
    mapper_->add(Mapping("file:size", m.file_content->column("size")));
    mapper_->add(Mapping("source:_name", m.sources->column("name")));
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
    std::string qry("DELETE FROM bdb_files WHERE uuid = :uuid");
    sql::BindMap binds;
    binds.add(":uuid", Variant(entry.uuid()));
    shared_ptr<sql::Result> r = conn()->execute(sql::Query(qry, binds));
    return r->affected_rows();
}

std::vector<oh5::Source>
RelationalDatabase::do_sources() const {
    return RdbHelper(conn()).select_all_sources();
}

void
RelationalDatabase::do_add_source(const oh5::Source& source) {
    RdbHelper(conn()).add_source(source);
}

void
RelationalDatabase::do_update_source(const oh5::Source& source) {
    RdbHelper(conn()).update_source(source);
}

void
RelationalDatabase::do_remove_source(const oh5::Source& source) {
    RdbHelper(conn()).remove_source(source);
}

} // namespace rdb
} // namespace db
} // namespace brfc
