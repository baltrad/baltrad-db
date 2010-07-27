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

#include <brfc/rdb/SaveGroup.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/GroupCache.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Result.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace rdb {

SaveGroup::SaveGroup(RelationalDatabase* rdb,
                     GroupCache* cache)
        : rdb_(rdb)
        , cache_(cache)
        , stmt_()
        , special_(rdb->mapper()->specializations_on(Model::instance().groups)) {
}

void
SaveGroup::operator()(const oh5::Group& group) {
    stmt_ = sql::Insert::create(Model::instance().groups);
    if (rdb_->connection().has_feature(sql::Connection::RETURNING))
        stmt_->add_return(stmt_->table()->column("id"));

    bind_plain(group);
    bind_specializations(group);

    shared_ptr<sql::Result> result = rdb_->connection().execute(*stmt_);

    cache_->insert(last_id(*result), group.shared_from_this());
}


void
SaveGroup::bind_plain(const oh5::Group& group) {
    Variant file_id;
    GroupCache::OptionalKey parent_id;
    shared_ptr<const oh5::Group> parent = group.parent<oh5::Group>();
    if (parent) {
        parent_id = cache_->key(parent);
    }
    if (group.file())
        file_id = Variant(rdb_->db_id(*group.file()));

    sql::Factory xpr;
    if (parent_id)
        stmt_->value("parent_id", xpr.int64_(parent_id.get()));
    stmt_->value("file_id", xpr.literal(file_id));
    stmt_->value("name", xpr.string(group.name()));
}

void
SaveGroup::bind_specializations(const oh5::Group& group) {
    sql::Factory xpr;
    BOOST_FOREACH(const Mapping& mapping, special_) {
        shared_ptr<const oh5::Attribute> attr =
            group.child_attribute(mapping.attribute);
        if (attr)
            stmt_->value(mapping.column, xpr.literal(attr->value()));
    }
    // XXX: note that this relies on implicit convertion of attribute value
    //      in DB (True/False -> bool; ISO 8601 date/time strings)
    //
    // XXX: this should be explicit
}

long long
SaveGroup::last_id(sql::Result& result) {
    if (rdb_->connection().has_feature(sql::Connection::RETURNING) and result.next()) {
        return result.value_at(0).int64_();
    } else {
        // XXX: last insert id!
        return 0;
    }
}

} // namespace rdb
} // namespace brfc
