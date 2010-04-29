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

#include <brfc/rdb/SaveFile.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>

#include <brfc/FileHasher.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/rdb/Connection.hpp>

namespace brfc {
namespace rdb {

SaveFile::SaveFile(RelationalDatabase* rdb)
        : rdb_(rdb)
        , group_id_cache_(rdb)
        , save_attribute_(rdb, &group_id_cache_)
        , save_group_(rdb, &group_id_cache_) {
}

void
SaveFile::operator()(const oh5::Group& group) {
    save_group_(group);
}

void
SaveFile::operator()(const oh5::Attribute& attribute) {
    save_attribute_(attribute);
}

long long
SaveFile::operator()(const oh5::File& file,
                     const QString& proposed_filename,
                     unsigned int filename_version) {
    QStringList columns, binds;
    columns.append("source_id");
    columns.append("unique_id");

    const MappingVector& special = rdb_->mapper().specializations_on("bdb_files");
    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: get rid of this
        if (mapping.attribute == "file_id") 
            continue;
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const QString& column, columns) {
        binds.append(":" + column);
    }

    QString stmt("INSERT INTO bdb_files(" + columns.join(", ") +
                 ", hash_type, proposed_filename, filename_version) "
                 "VALUES(" + binds.join(", ") +
                 ", :hash_type, :proposed_filename, :filename_version)");
    if (rdb_->supports_returning())
        stmt += " RETURNING id";

    SqlQuery qry(stmt);

    if (not file.source())
        throw db_error("no Source associated with File");

    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: do something about it, we can't continue adding here
        if (mapping.attribute == "path" || mapping.attribute == "file_id")
            continue;
        const Variant& value = 
                file.root()->child_attribute(mapping.attribute)->value();
        qry.binds().add(":" + mapping.column, value);
    }

    qry.binds().add(":path", Variant(file.path()));
    qry.binds().add(":hash_type", Variant(rdb_->file_hasher().name()));
    qry.binds().add(":unique_id", Variant(rdb_->file_hasher().hash(file)));
    qry.binds().add(":source_id", Variant(rdb_->db_id(*file.source())));
    qry.binds().add(":proposed_filename", Variant(proposed_filename));
    qry.binds().add(":filename_version", Variant(filename_version));

    shared_ptr<ResultSet> result = rdb_->connection().execute(qry);

    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        visit(node, *this);
    }

    if (rdb_->supports_returning() and result->next()) {
        return result->integer(0);
    } else {
        // XXX: last insert id!
        return 0;
    }
}



} // namespace rdb
} // namespace brfc
