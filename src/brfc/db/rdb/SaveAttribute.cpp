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

#include <brfc/db/rdb/SaveAttribute.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/GroupCache.hpp>
#include <brfc/db/rdb/Model.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace db {
namespace rdb {

SaveAttribute::SaveAttribute(RelationalDatabase* rdb,
                             GroupCache* group_cache)
        : rdb_(rdb)
        , group_cache_(group_cache) {
}

void
SaveAttribute::operator()(const oh5::Attribute& attr) {
    sql::InsertPtr stmt;
    if (not rdb_->mapper()->has(attr.full_name())) {
        stmt = invalid_attribute_query(attr);
    } else if (rdb_->mapper()->is_specialized(attr.full_name())) {
        // ignore specialized attributes
    } else {
        stmt = valid_attribute_query(attr);
    }

    if (stmt) {
        rdb_->connection().execute(*stmt);
    }
}

sql::InsertPtr
SaveAttribute::invalid_attribute_query(const oh5::Attribute& attr) {
    sql::InsertPtr stmt = sql::Insert::create(Model::instance().invalid_attrs);

    GroupCache::OptionalKey grp_id;
    if (attr.parent<oh5::Group>())
        grp_id = group_cache_->key(attr.parent<oh5::Group>());
    
    sql::Factory xpr;
    stmt->value("name", xpr.string(attr.full_name()));
    if (grp_id)
        stmt->value("group_id", xpr.int64_(grp_id.get()));
    return stmt;
}

sql::InsertPtr
SaveAttribute::valid_attribute_query(const oh5::Attribute& attr) {
    const Mapping& mapping = rdb_->mapper()->mapping(attr.full_name());
    sql::InsertPtr stmt = sql::Insert::create(
        dynamic_pointer_cast<sql::Table>(mapping.column->selectable()));

    GroupCache::OptionalKey grp_id;
    if (attr.parent<oh5::Group>())
        grp_id = group_cache_->key(attr.parent<oh5::Group>());
    
    sql::Factory xpr;
    if (grp_id)
        stmt->value("group_id", xpr.int64_(grp_id.get()));
    stmt->value("attribute_id", xpr.int64_(mapping.id));
    stmt->value("value", attr_sql_value(attr));

    return stmt;
}

sql::LiteralPtr
attr_sql_value(const oh5::Attribute& attr) {
    // XXX: note that this relies on implicit conversion of attribute value
    //      in DB (True/False -> bool; ISO 8601 date/time strings)
    // XXX: this should be explicit

    switch (attr.value().type()) {
        case oh5::Scalar::STRING:
            return sql::Literal::create(Variant(attr.value().string()));
        case oh5::Scalar::INT64:
            return sql::Literal::create(Variant(attr.value().int64_()));
        case oh5::Scalar::DOUBLE:
            return sql::Literal::create(Variant(attr.value().double_()));
    }
    throw std::runtime_error("");
}

} // namespace rdb
} // namespace db
} // namespace brfc
