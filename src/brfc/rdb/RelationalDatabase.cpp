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

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/BindMap.hpp>
#include <brfc/rdb/Compiler.hpp>
#include <brfc/rdb/PostgresConnection.hpp>
#include <brfc/rdb/QueryToSelect.hpp>
#include <brfc/rdb/SaveFile.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <iostream>

namespace brfc {
namespace rdb {

RelationalDatabase::RelationalDatabase(const QString& dsn_)
        : conn_()
        , mapper_(new AttributeMapper())
        , specs_(new oh5::AttributeSpecs())
        , file_hasher_() {

    QUrl dsn(dsn_);
    
    conn_.reset(new PostgresConnection(dsn));
}

bool
RelationalDatabase::supports_returning() const {
    return conn_->has_feature(Connection::RETURNING);
}

RelationalDatabase::~RelationalDatabase() {

}

shared_ptr<const oh5::AttributeSpecs>
RelationalDatabase::specs() const {
    return specs_;
}

shared_ptr<oh5::AttributeSpecs>
RelationalDatabase::specs() {
    return specs_;
}

const AttributeMapper&
RelationalDatabase::mapper() const {
    return *mapper_;
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
    QString qry("SELECT true FROM bdb_files WHERE unique_id = :unique_id");
    BindMap binds;
    binds.add(":unique_id", file_hasher().hash(file));
    shared_ptr<ResultSet> result = connection().execute(qry, binds);
    return result->size() > 0;
}

long long
RelationalDatabase::do_save_file(const oh5::File& file,
                                 const QString& proposed_filename,
                                 unsigned int filename_version) {
    SaveFile save(this);
    return save(file, proposed_filename, filename_version);
}

unsigned int
RelationalDatabase::do_next_filename_version(const QString& filename) {
    QString qry("SELECT MAX(filename_version) + 1 "
                "FROM bdb_files "
                "WHERE proposed_filename = :filename");
    BindMap binds;
    binds.add(":filename", filename);
    shared_ptr<ResultSet> result = connection().execute(qry, binds);
    result->next();
    if (result->is_null(0)) {
        return 0;
    } else {
        return static_cast<unsigned int>(result->integer(0));
    }
}

shared_ptr<ResultSet>
RelationalDatabase::do_query(const Query& query) {
    SelectPtr select = QueryToSelect::transform(query, *mapper_.get());

    Compiler compiler;
    compiler.compile(*select);

    return this->query(compiler.compiled(), compiler.binds());
}


long long
RelationalDatabase::do_db_id(const oh5::File& file) {
    QString qry("SELECT id FROM bdb_files WHERE unique_id = :unique_id");
    BindMap binds;
    binds.add(":unique_id", file_hasher().hash(file));
    shared_ptr<ResultSet> r = query(qry, binds);
    r->next();
    return r->integer(0);
}

long long
RelationalDatabase::db_id(const oh5::Source& src) {
    QString qry("SELECT id FROM bdb_sources WHERE node_id = :node_id");
    BindMap binds;
    binds.add(":node_id", src.node_id());
    shared_ptr<ResultSet> r = query(qry, binds);
    if (r->next())
        return r->integer(0);
    else
        return 0;
}

shared_ptr<oh5::SourceCentre>
RelationalDatabase::load_source_centre(shared_ptr<oh5::SourceCentre> src,
                                       long long id) {
    QStringList wcl;
    BindMap binds;

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

    QString qstr = QString("SELECT bdb_sources.id, node_id, country_code, ") +
                   QString("originating_centre, wmo_cccc ") +
                   QString("FROM bdb_source_centres ") +
                   QString("JOIN bdb_sources ON bdb_sources.id = bdb_source_centres.id ") +
                   QString("WHERE ") + wcl.join(" OR ");
    
    shared_ptr<ResultSet> result = connection().execute(qstr, binds);
    
    if (result->size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().toStdString());
    } else if (result->size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().toStdString());
    }
    result->next();

    id = result->integer(0);
    SourceMap::iterator i = sources_.find(id);

    if (i == sources_.end()) {
        src->node_id(result->string(1));
        src->country_code(result->integer(2));
        src->originating_centre(result->integer(3));
        src->wmo_cccc(result->string(4));
        i = sources_.insert(std::make_pair(id, src)).first;
    }
    return static_pointer_cast<oh5::SourceCentre>(i->second);
}

shared_ptr<oh5::SourceRadar>
RelationalDatabase::load_source_radar(shared_ptr<oh5::SourceRadar> src) {
    QStringList wcl;
    BindMap binds;

    // wmo_code or radar_site is required for radar sources
    if (src->wmo_code()) {
        wcl.append("wmo_code = :wmo_code");
        binds.add(":wmo_code", src->wmo_code());
    }
    if (src->radar_site() != "") {
        wcl.append("radar_site = :radar_site");
        binds.add(":radar_site", Variant(src->radar_site()));
    }
    if (src->place() != "") {
        wcl.append("place = :place");
        binds.add(":place", src->place());
    }

    QString qstr = QString("SELECT bdb_sources.id, node_id, centre_id, ") +
                   QString("wmo_code, radar_site, place ") +
                   QString("FROM bdb_source_radars ") +
                   QString("JOIN bdb_sources ON bdb_source_radars.id = bdb_sources.id ") +
                   QString("WHERE ") + wcl.join(" OR ");
    shared_ptr<ResultSet> result = connection().execute(qstr, binds);

    if (result->size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().toStdString());
    } else if (result->size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().toStdString());
    }
    
    result->next();

    long long id = result->integer(0);
    SourceMap::iterator i = sources_.find(id);

    if (i == sources_.end()) {
        shared_ptr<oh5::SourceCentre> centre = make_shared<oh5::SourceCentre>();
        centre = load_source_centre(centre, result->integer(2));

        src->centre(centre);
        src->node_id(result->string(1));
        src->wmo_code(result->integer(3));
        src->radar_site(result->string(4));
        src->place(result->string(5));
        i = sources_.insert(std::make_pair(id, src)).first;
    }

    return static_pointer_cast<oh5::SourceRadar>(i->second);
}

shared_ptr<oh5::Source>
RelationalDatabase::do_load_source(const QString& srcstr) {

    shared_ptr<oh5::Source> src = oh5::Source::from_source_attribute(srcstr);
    shared_ptr<oh5::SourceRadar> radar;
    shared_ptr<oh5::SourceCentre> centre;

    if (radar = dynamic_pointer_cast<oh5::SourceRadar>(src)) {
        return load_source_radar(radar);
    } else if (centre = dynamic_pointer_cast<oh5::SourceCentre>(src)) {
        return load_source_centre(centre);
    } else {
        throw lookup_error(srcstr.toUtf8().constData());
    }

}

shared_ptr<ResultSet>
RelationalDatabase::query(const QString& query_str,
                          const BindMap& binds) {
    return connection().execute(query_str, binds);
}

void
RelationalDatabase::populate_mapper_and_specs() {
    mapper_->clear();
    specs_->clear();
    shared_ptr<ResultSet> r = query("SELECT id, name, converter, "
                                    "storage_table, storage_column, "
                                    "ignore_in_hash "
                                    "FROM bdb_attributes", BindMap());
    while (r->next()) {
        mapper_->add(Mapping(r->integer(0), // id
                             r->string(1),  // name
                             r->string(3),  // table
                             r->string(4))); // column
        specs_->add(oh5::AttributeSpec(r->string(1), // name
                                       r->string(2), // typename
                                       r->boolean(5))); // ignored in hash
    }
}

void
RelationalDatabase::do_remove_file(const QString& path) {
    QString qry("DELETE FROM bdb_files WHERE path = :path");
    BindMap binds;
    binds.add(":path", path);
    connection().execute(qry, binds);
}

} // namespace rdb
} // namespace brfc
