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

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/String.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/File.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Connection.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/QueryToSelect.hpp>
#include <brfc/rdb/Result.hpp>
#include <brfc/rdb/RelationalResultSet.hpp>
#include <brfc/rdb/SaveFile.hpp>
#include <brfc/rdb/SHA1AttributeHasher.hpp>

#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace rdb {

RelationalDatabase::RelationalDatabase(const String& dsn_)
        : conn_(Connection::create(dsn_))
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

void
RelationalDatabase::do_begin() {
    connection().begin();
}

void
RelationalDatabase::do_rollback() {
    connection().rollback();
}

void
RelationalDatabase::do_commit() {
    connection().commit();
}

bool
RelationalDatabase::do_has_file(const oh5::File& file) {
    String qry("SELECT true FROM bdb_files WHERE unique_id = :unique_id");
    sql::BindMap binds;
    binds.add(":unique_id", Variant(file_hasher().hash(file)));
    shared_ptr<Result> result = connection().execute(qry, binds);
    return result->size() > 0;
}

long long
RelationalDatabase::do_save_file(const oh5::File& file,
                                 const String& proposed_filename,
                                 unsigned int filename_version) {
    SaveFile save(this);
    return save(file, proposed_filename, filename_version);
}

unsigned int
RelationalDatabase::do_next_filename_version(const String& filename) {
    String qry("SELECT MAX(filename_version) + 1 "
                "FROM bdb_files "
                "WHERE proposed_filename = :filename");
    sql::BindMap binds;
    binds.add(":filename", Variant(filename));
    shared_ptr<Result> result = connection().execute(qry, binds);
    result->next();
    if (result->value_at(0).is_null()) {
        return 0;
    } else {
        return static_cast<unsigned int>(result->value_at(0).int64_());
    }
}

shared_ptr<ResultSet>
RelationalDatabase::do_query(const Query& query) {
    sql::SelectPtr select = QueryToSelect::transform(query, *mapper_.get());

    sql::Compiler compiler;
    compiler.compile(*select);

    shared_ptr<Result> res = connection().execute(compiler.compiled(),
                                                  compiler.binds());
    return shared_ptr<ResultSet>(new RelationalResultSet(res));
}


long long
RelationalDatabase::do_db_id(const oh5::File& file) {
    String qry("SELECT id FROM bdb_files WHERE unique_id = :unique_id");
    sql::BindMap binds;
    binds.add(":unique_id", Variant(file_hasher().hash(file)));
    shared_ptr<Result> r = connection().execute(qry, binds);
    r->next();
    return r->value_at(0).int64_();
}

long long
RelationalDatabase::db_id(const oh5::Source& src) {
    String qry("SELECT id FROM bdb_sources WHERE node_id = :node_id");
    sql::BindMap binds;
    binds.add(":node_id", Variant(src.node_id()));
    shared_ptr<Result> r = connection().execute(qry, binds);
    if (r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

shared_ptr<oh5::SourceCentre>
RelationalDatabase::load_source_centre(shared_ptr<oh5::SourceCentre> src,
                                       long long id) {
    StringList wcl;
    sql::BindMap binds;

    // originating_centre or country_code is required for org. sources
    if (src->originating_centre()) {
        wcl.append("originating_centre = :org_centre");
        binds.add(":org_centre", Variant(src->originating_centre()));
    }
    if (src->country_code()) {
        wcl.append("country_code = :country_code");
        binds.add(":country_code", Variant(src->country_code()));
    }
    if (id) {
        wcl.append("bdb_sources.id = :id");
        binds.add(":id", Variant(id));
    }

    String qstr = String("SELECT bdb_sources.id, node_id, country_code, ") +
                   String("originating_centre, wmo_cccc ") +
                   String("FROM bdb_source_centres ") +
                   String("JOIN bdb_sources ON bdb_sources.id = bdb_source_centres.id ") +
                   String("WHERE ") + wcl.join(" OR ");
    
    shared_ptr<Result> r = connection().execute(qstr, binds);
    
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
    StringList wcl;
    sql::BindMap binds;

    // wmo_code or radar_site is required for radar sources
    if (src->wmo_code()) {
        wcl.append("wmo_code = :wmo_code");
        binds.add(":wmo_code", Variant(src->wmo_code()));
    }
    if (src->radar_site() != "") {
        wcl.append("radar_site = :radar_site");
        binds.add(":radar_site", Variant(src->radar_site()));
    }
    if (src->place() != "") {
        wcl.append("place = :place");
        binds.add(":place", Variant(src->place()));
    }

    String qstr = String("SELECT bdb_sources.id, node_id, centre_id, ") +
                   String("wmo_code, radar_site, place ") +
                   String("FROM bdb_source_radars ") +
                   String("JOIN bdb_sources ON bdb_source_radars.id = bdb_sources.id ") +
                   String("WHERE ") + wcl.join(" OR ");
    shared_ptr<Result> r = connection().execute(qstr, binds);

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
    String qry("SELECT id, name, converter, "
                      "storage_table, storage_column, "
                      "ignore_in_hash "
               "FROM bdb_attributes");
    shared_ptr<Result> r = connection().execute(qry);

    const Model& model = Model::instance();
    String table, column;
    while (r->next()) {
        table = r->value_at("storage_table").string();
        column = r->value_at("storage_column").string();
        Mapping mapping(r->value_at("id").int64_(),
                        r->value_at("name").string(),
                        r->value_at("converter").string(),
                        model.table_by_name(table)->column(column),
                        r->value_at("ignore_in_hash").bool_());
        mapper_->add(mapping);
    }
}

void
RelationalDatabase::do_remove_file(const String& path) {
    String qry("DELETE FROM bdb_files WHERE path = :path");
    sql::BindMap binds;
    binds.add(":path", Variant(path));
    connection().execute(qry, binds);
}

} // namespace rdb
} // namespace brfc
