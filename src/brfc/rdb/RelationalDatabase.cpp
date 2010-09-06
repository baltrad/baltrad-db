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

#include <brfc/rdb/RelationalDatabase.hpp>

#include <deque>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Query.hpp>
#include <brfc/String.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/QueryToSelect.hpp>
#include <brfc/rdb/RelationalFileEntry.hpp>
#include <brfc/rdb/SaveFile.hpp>
#include <brfc/rdb/SHA1AttributeHasher.hpp>

#include <brfc/sql/expr.hpp>
#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

#include <iostream>

namespace brfc {
namespace rdb {

RelationalDatabase::RelationalDatabase(const String& dsn_)
        : conn_(sql::Connection::create(dsn_))
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher(mapper_)) {
    conn_->open();
}

RelationalDatabase::~RelationalDatabase() {

}

shared_ptr<AttributeMapper>
RelationalDatabase::mapper() {
    return mapper_;
}

shared_ptr<const AttributeMapper>
RelationalDatabase::mapper() const {
    return mapper_;
}

void
RelationalDatabase::file_hasher(shared_ptr<FileHasher> hasher) {
    file_hasher_.swap(hasher);
}

void
RelationalDatabase::file_hasher(FileHasher* hasher) {
    file_hasher(shared_ptr<FileHasher>(hasher, no_delete));
}

bool
RelationalDatabase::do_has_file(const oh5::File& file) {
    const Model& m = Model::instance();

    oh5::Source src = load_source(file.what_source());
    const String& hash = file_hasher().hash(file, src);

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create();
    qry->what(xpr.bool_(true));
    qry->from(m.files);
    qry->where(m.files->column("unique_id")->eq(xpr.string(hash)));

    shared_ptr<sql::Result> result = connection().execute(*qry);
    return result->size() > 0;
}

shared_ptr<FileEntry>
RelationalDatabase::do_save_file(const oh5::File& file) {
    SaveFile save(this);
    long long id = 0;
    connection().begin();
    try {
        id = save(file);
        connection().commit();
    } catch (const std::runtime_error&) {
        connection().rollback();
        throw;
    }
    shared_ptr<FileEntry> entry(new RelationalFileEntry(id, conn_));
    return entry;
}

shared_ptr<ResultSet>
RelationalDatabase::do_query(const Query& query) {
    sql::SelectPtr select = QueryToSelect::transform(query, *mapper_.get());
    shared_ptr<sql::Result> res = connection().execute(*select);
    shared_ptr<ResultSet> rset(new ResultSet());
    while (res->next()) {
        long long id = res->value_at(0).int64_();
        shared_ptr<FileEntry> entry(new RelationalFileEntry(id, conn_));
        rset->add(entry);
    }
    return rset;
}

long long
RelationalDatabase::db_id(const oh5::File& file) {
    const Model& m = Model::instance();

    oh5::Source src = load_source(file.what_source());
    const String& hash = file_hasher().hash(file, src);

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create();
    qry->what(m.files->column("id"));
    qry->from(m.files);
    qry->where(m.files->column("unique_id")->eq(xpr.string(hash)));

    shared_ptr<sql::Result> r = connection().execute(*qry);
    r->next();
    return r->value_at(0).int64_();
}

long long
RelationalDatabase::db_id(const oh5::Source& src) {
    const Model& m = Model::instance();
    sql::Factory xpr;

    sql::SelectPtr qry = sql::Select::create();
    qry->distinct(true);
    qry->what(m.sources->column("id"));
    qry->from(m.sources->join(m.source_kvs));
    qry->where(xpr.bool_(false));
    
    sql::ExpressionPtr x;
    BOOST_FOREACH(const String& key, src.keys()) {
        x = m.source_kvs->column("key")->eq(xpr.string(key));
        x = x->and_(m.source_kvs->column("value")->eq(xpr.string(src.at(key))));
        qry->where(qry->where()->or_(x->parentheses()));
    }

    shared_ptr<sql::Result> r = connection().execute(*qry);
    if (r->size() == 1 and r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

oh5::Source
RelationalDatabase::load_source(const String& srcstr) {
    const Model& m = Model::instance();
    sql::Factory xpr;

    oh5::Source src = oh5::Source::from_string(srcstr);

    long long id = db_id(src);

    sql::SelectPtr qry = sql::Select::create(m.source_kvs);
    qry->where(m.source_kvs->column("source_id")->eq(xpr.int64_(id)));

    shared_ptr<sql::Result> r = connection().execute(*qry);

    src.clear();
    
    while (r->next()) {
        src.add(r->value_at("key").string(), r->value_at("value").string());
    }

    qry = sql::Select::create(m.sources);
    qry->where(m.sources->column("id")->eq(xpr.int64_(id)));

    r = connection().execute(*qry);

    r->next();
    src.add("name", r->value_at("name").string());

    return src;
}

void
RelationalDatabase::populate_mapper() {
    mapper_->clear();

    const Model& m = Model::instance();
    sql::SelectPtr qry = sql::Select::create(m.attrs);
    shared_ptr<sql::Result> r = connection().execute(*qry);

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
}

void
RelationalDatabase::do_remove_file(const FileEntry& entry) {
    String qry("DELETE FROM bdb_files WHERE id = :id");
    sql::BindMap binds;
    binds.add(":id", Variant(entry.id()));
    connection().execute(qry, binds);
}

} // namespace rdb
} // namespace brfc
