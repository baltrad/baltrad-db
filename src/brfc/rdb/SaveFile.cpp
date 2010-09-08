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

#include <brfc/FileHasher.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/rdb/RelationalFileEntry.hpp>
#include <brfc/rdb/Model.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/LargeObject.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Result.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace rdb {

SaveFile::SaveFile(RelationalDatabase* rdb)
        : rdb_(rdb)
        , group_cache_(rdb)
        , save_attribute_(rdb, &group_cache_)
        , save_group_(rdb, &group_cache_) {
}

void
SaveFile::operator()(const oh5::Group& group) {
    save_group_(group);
}

void
SaveFile::operator()(const oh5::Attribute& attribute) {
    save_attribute_(attribute);
}

shared_ptr<RelationalFileEntry>
SaveFile::operator()(const oh5::File& file) {
    sql::TablePtr files = Model::instance().files;
    const MappingVector& special = rdb_->mapper()->specializations_on(files);

    sql::InsertPtr stmt = sql::Insert::create(files);

    if (rdb_->connection().has_feature(sql::Connection::RETURNING))
        stmt->add_return(files->column("id"));
    
    sql::Factory xpr;
    BOOST_FOREACH(const Mapping& mapping, special) {
        if (mapping.attribute.starts_with("file:"))
            continue;
        const Variant& value = 
                file.root()->child_attribute(mapping.attribute)->value();
        stmt->value(mapping.column, xpr.literal(value));
        // XXX: note that this relies on implicit convertion of attribute value
        //      in DB (True/False -> bool; ISO 8601 date/time strings)
        //
        // XXX: this should be explicit
    }

    stmt->value("hash_type", xpr.string(rdb_->file_hasher().name()));
    stmt->value("unique_id", xpr.string(rdb_->file_hasher().hash(file)));
    stmt->value("source_id", xpr.int64_(rdb_->db_id(file.source())));

    shared_ptr<sql::Result> result = rdb_->connection().execute(*stmt);

    long long file_id = 0;
    if (rdb_->connection().has_feature(sql::Connection::RETURNING) and result->next()) {
        file_id = result->value_at(0).int64_();
    } else {
        // XXX: last insert id!
    }

    save_group_.file_id(file_id);

    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        visit(node, *this);
    }

    // transfer the file to database
    shared_ptr<sql::LargeObject> lo = rdb_->connection().large_object(file.path());

    stmt = sql::Insert::create(Model::instance().file_content);
    stmt->value("file_id", xpr.int64_(file_id));
    stmt->value("lo_id", xpr.int64_(lo->id()));

    rdb_->connection().execute(*stmt);

    return shared_ptr<RelationalFileEntry>(
                new RelationalFileEntry(rdb_->connection_ptr(),
                                        file_id,
                                        lo->id()));
}



} // namespace rdb
} // namespace brfc
