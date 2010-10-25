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
}

RelationalDatabase::RelationalDatabase(shared_ptr<sql::Connection> conn,
                                       shared_ptr<RdbHelper> helper)
        : conn_(conn)
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher())
        , helper_(helper) {
    BRFC_ASSERT(conn);
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
            m.files->column("unique_id")->eq(xpr.string(hash)),
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

shared_ptr<FileResult>
RelationalDatabase::do_query(const FileQuery& query) {
    sql::SelectPtr select = QueryToSelect::transform(query, mapper());
    shared_ptr<sql::Result> res = conn().execute(*select);
    shared_ptr<FileResult> rset(new FileResult());
    while (res->next()) {
        long long id = res->value_at(0).int64_();
        long long lo_id = res->value_at(1).int64_();
        shared_ptr<RdbFileEntry> entry(new RdbFileEntry(this, id));
        entry->lo_id(lo_id);
        rset->add(entry);
    }
    return rset;
}

shared_ptr<AttributeResult>
RelationalDatabase::do_query(const AttributeQuery& query) {
    sql::SelectPtr select = QueryToSelect::transform(query, mapper());
    shared_ptr<sql::Result> res = conn().execute(*select);
    return shared_ptr<AttributeResult>(new RdbAttributeResult(res));
}

void
RelationalDatabase::populate_mapper() {
    mapper_->clear();

    const Model& m = Model::instance();
    sql::SelectPtr qry = sql::Select::create(m.attrs);
    shared_ptr<sql::Result> r = conn().execute(*qry);

    String table, column;
    while (r->next()) {
        table = r->value_at("storage_table").string();
        column = r->value_at("storage_column").string();
        Mapping mapping(r->value_at("id").int64_(),
                        r->value_at("name").string(),
                        r->value_at("converter").string(),
                        m.table_by_name(table)->column(column),
                        r->value_at("ignore_in_hash").bool_());
        mapper_->add(mapping);
    }
    populate_hasher();
}

void
RelationalDatabase::populate_hasher() {
    file_hasher_->clear_ignored();
    file_hasher_->ignore(mapper_->ignored_in_hash());
}

bool
RelationalDatabase::do_remove(const FileEntry& entry) {
    String qry("DELETE FROM bdb_files WHERE id = :id");
    sql::BindMap binds;
    binds.add(":id", Variant(entry.id()));
    shared_ptr<sql::Result> r = conn().execute(qry, binds);
    return r->affected_rows();
}

} // namespace rdb
} // namespace db
} // namespace brfc
