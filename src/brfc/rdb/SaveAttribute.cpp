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

#include <brfc/rdb/SaveAttribute.hpp>

#include <QtSql/QSqlError>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/GroupIdCache.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace rdb {

SaveAttribute::SaveAttribute(RelationalDatabase* rdb,
                             GroupIdCache* group_id_cache)
        : rdb_(rdb)
        , mapper_(&rdb->mapper())
        , group_id_cache_(group_id_cache)
        , queries_() {
}

void
SaveAttribute::operator()(const oh5::Attribute& attr) {
    QSqlQuery* qry = 0;
    if (not attr.is_valid()) {
        qry = &invalid_attribute_query(attr);
    } else if (mapper_->is_specialized(attr.full_name())) {
        // ignore specialized attributes
    } else {
        qry = &valid_attribute_query(attr);
    }

    if (qry != 0 and not qry->exec())
        throw db_error(qry->lastError());
}

QSqlQuery&
SaveAttribute::invalid_attribute_query(const oh5::Attribute& attr) {
    QueryMap::iterator iter = queries_.find("bdb_invalid_attributes");
    if (iter == queries_.end()) {
        QSqlQuery qry(rdb_->connection());
        qry.prepare("INSERT INTO bdb_invalid_attributes(name, group_id) "
                    "VALUES (:name, :group_id)");
        QueryMap::value_type val("bdb_invalid_attributes", qry);
        iter = queries_.insert(val).first;
    }
    QSqlQuery& qry = iter->second;

    qry.bindValue("name", attr.full_name());
    GroupIdCache::OptionalId grp_id;
    if (attr.parent_group())
        grp_id = group_id_cache_->get(*attr.parent_group());
    qry.bindValue("group_id", grp_id ? grp_id.get() : QVariant());

    return qry;
}

QSqlQuery&
SaveAttribute::valid_attribute_query(const oh5::Attribute& attr) {
    const Mapping& mapping = mapper_->mapping(attr.full_name());
    QueryMap::iterator iter = queries_.find(mapping.table);
    if (iter == queries_.end()) {
        QSqlQuery qry(rdb_->connection()); 
        qry.prepare("INSERT INTO " + mapping.table +
                    "(group_id, attribute_id, value) " +
                    "VALUES(:group_id, :attribute_id, :value)");
        QueryMap::value_type val(mapping.table, qry);
        iter = queries_.insert(val).first;
    }
    QSqlQuery& qry = iter->second;

    GroupIdCache::OptionalId grp_id;
    if (attr.parent_group())
        grp_id = group_id_cache_->get(*attr.parent_group());
    qry.bindValue("group_id", grp_id ? grp_id.get() : QVariant());
    qry.bindValue("attribute_id", mapping.id);
    qry.bindValue("value", attr.value().to_qvariant());
    
    return qry;
}

} // namespace rdb
} // namespace brfc
