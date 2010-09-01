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
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

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
    const String& hash = file_hasher().hash(file);

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
    const String& hash = file_hasher().hash(file);

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
    qry->what(m.sources->column("id"));
    qry->from(m.sources);
    qry->where(m.sources->column("node_id")->eq(xpr.string(src.node_id())));

    shared_ptr<sql::Result> r = connection().execute(*qry);
    if (r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

shared_ptr<oh5::SourceCentre>
RelationalDatabase::load_source_centre(shared_ptr<oh5::SourceCentre> src,
                                       long long id) {
    Model m = Model::instance();

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create(m.source_centres->join(m.sources));
    qry->where(xpr.bool_(false));

    sql::ExpressionPtr x;
    // originating_centre or country_code is required for org. sources
    if (src->originating_centre()) {
        x = m.source_centres->column("originating_centre");
        x = x->eq(xpr.int64_(src->originating_centre()));
        qry->where(qry->where()->or_(x));
    }
    if (src->country_code()) {
        x = m.source_centres->column("country_code");
        x = x->eq(xpr.int64_(src->country_code()));
        qry->where(qry->where()->or_(x));
    }
    if (id) {
        x = m.source_centres->column("id")->eq(xpr.int64_(id));
        qry->where(qry->where()->or_(x));
    }

    shared_ptr<sql::Result> r = connection().execute(*qry);
    
    if (r->size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().to_std_string());
    } else if (r->size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().to_std_string());
    }
    r->next();

    id = r->value_at("id").int64_();
    SourceMap::iterator i = sources_.find(id);

    if (i == sources_.end()) {
        src->node_id(r->value_at("node_id").string());
        src->country_code(r->value_at("country_code").int64_());
        src->originating_centre(r->value_at("originating_centre").int64_());
        src->wmo_cccc(r->value_at("wmo_cccc").string());
        i = sources_.insert(std::make_pair(id, src)).first;
    }
    return static_pointer_cast<oh5::SourceCentre>(i->second);
}

shared_ptr<oh5::SourceRadar>
RelationalDatabase::load_source_radar(shared_ptr<oh5::SourceRadar> src) {
    Model m = Model::instance();

    sql::Factory xpr;
    sql::SelectPtr qry = sql::Select::create(m.source_radars->join(m.sources));
    qry->where(xpr.bool_(false));

    sql::ExpressionPtr x;
    // wmo_code or radar_site is required for radar sources
    if (src->wmo_code()) {
        x = m.source_radars->column("wmo_code");
        x = x->eq(xpr.int64_(src->wmo_code()));
        qry->where(qry->where()->or_(x));
    }
    if (src->radar_site() != "") {
        x = m.source_radars->column("radar_site");
        x = x->eq(xpr.string(src->radar_site()));
        qry->where(qry->where()->or_(x));
    }
    if (src->place() != "") {
        x = m.source_radars->column("place");
        x = x->eq(xpr.string(src->place()));
        qry->where(qry->where()->or_(x));
    }

    shared_ptr<sql::Result> r = connection().execute(*qry);

    if (r->size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().to_std_string());
    } else if (r->size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().to_std_string());
    }
    
    r->next();

    long long id = r->value_at("id").int64_();
    SourceMap::iterator i = sources_.find(id);

    if (i == sources_.end()) {
        shared_ptr<oh5::SourceCentre> centre = make_shared<oh5::SourceCentre>();
        centre = load_source_centre(centre, r->value_at("centre_id").int64_());

        src->centre(centre);
        src->node_id(r->value_at("node_id").string());
        src->wmo_code(r->value_at("wmo_code").int64_());
        src->radar_site(r->value_at("radar_site").string());
        src->place(r->value_at("place").string());
        i = sources_.insert(std::make_pair(id, src)).first;
    }

    return static_pointer_cast<oh5::SourceRadar>(i->second);
}

shared_ptr<oh5::Source>
RelationalDatabase::do_load_source(const String& srcstr) {

    shared_ptr<oh5::Source> src = oh5::Source::from_source_attribute(srcstr);
    shared_ptr<oh5::SourceRadar> radar;
    shared_ptr<oh5::SourceCentre> centre;

    if (radar = dynamic_pointer_cast<oh5::SourceRadar>(src)) {
        return load_source_radar(radar);
    } else if (centre = dynamic_pointer_cast<oh5::SourceCentre>(src)) {
        return load_source_centre(centre);
    } else {
        throw lookup_error(srcstr.to_utf8());
    }

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
