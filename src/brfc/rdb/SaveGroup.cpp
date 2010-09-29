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
#include <brfc/rdb/SaveAttribute.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
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
        , file_id_(0)
        , stmt_()
        , special_(rdb->mapper()->specializations_on(Model::instance().groups)) {
}

void
SaveGroup::operator()(const oh5::Group& group) {
    stmt_ = sql::Insert::create(Model::instance().groups);
    if (rdb_->connection().dialect().has_feature(sql::Dialect::RETURNING))
        stmt_->add_return(stmt_->table()->column("id"));

    bind_plain(group);
    bind_specializations(group);

    shared_ptr<sql::Result> result = rdb_->connection().execute(*stmt_);

    cache_->insert(last_id(*result), &group);
}


void
SaveGroup::bind_plain(const oh5::Group& group) {
    GroupCache::OptionalKey parent_id;
    const oh5::Group* parent = group.parent<oh5::Group>();
    if (parent) {
        parent_id = cache_->key(parent);
    }

    sql::Factory xpr;
    if (parent_id)
        stmt_->value("parent_id", xpr.int64_(parent_id.get()));
    stmt_->value("file_id", xpr.int64_(file_id_));
    stmt_->value("name", xpr.string(group.name()));
}

void
SaveGroup::bind_specializations(const oh5::Group& group) {
    BOOST_FOREACH(const Mapping& mapping, special_) {
        const oh5::Attribute* attr =
            group.attribute(mapping.attribute);
        if (attr)
            stmt_->value(mapping.column, attr_sql_value(*attr));
    }
}

long long
SaveGroup::last_id(sql::Result& result) {
    const sql::Dialect& d = rdb_->connection().dialect();
    if (d.has_feature(sql::Dialect::RETURNING) and result.next()) {
        return result.value_at(0).int64_();
    } else if (d.has_feature(sql::Dialect::LAST_INSERT_ID)) {
        return rdb_->connection().last_insert_id();
    } else {
        throw db_error("could not determine inserted group id");
    }
}

} // namespace rdb
} // namespace brfc
