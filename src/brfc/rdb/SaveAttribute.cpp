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

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Connection.hpp>
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
    SqlQuery* qry = 0;
    if (not attr.is_valid()) {
        qry = &invalid_attribute_query(attr);
    } else if (mapper_->is_specialized(attr.full_name())) {
        // ignore specialized attributes
    } else {
        qry = &valid_attribute_query(attr);
    }

    if (qry != 0)
        rdb_->connection().execute(*qry);
}

SqlQuery&
SaveAttribute::invalid_attribute_query(const oh5::Attribute& attr) {
    QueryMap::iterator iter = queries_.find("bdb_invalid_attributes");
    if (iter == queries_.end()) {
        String stmt("INSERT INTO bdb_invalid_attributes(name, group_id) "
                     "VALUES (:name, :group_id)");
        SqlQuery qry(stmt);
        QueryMap::value_type val("bdb_invalid_attributes", qry);
        iter = queries_.insert(val).first;
    }
    SqlQuery& qry = iter->second;

    GroupIdCache::OptionalId grp_id;
    if (attr.parent_group())
        grp_id = group_id_cache_->get(*attr.parent_group());

    qry.binds().add("name", Variant(attr.full_name()));
    qry.binds().add("group_id", grp_id ? Variant(grp_id.get())
                                       : Variant());

    return qry;
}

SqlQuery&
SaveAttribute::valid_attribute_query(const oh5::Attribute& attr) {
    const Mapping& mapping = mapper_->mapping(attr.full_name());
    QueryMap::iterator iter = queries_.find(mapping.table);
    if (iter == queries_.end()) {
        SqlQuery qry("INSERT INTO " + mapping.table +
                     "(group_id, attribute_id, value) " +
                     "VALUES(:group_id, :attribute_id, :value)");
        QueryMap::value_type val(mapping.table, qry);
        iter = queries_.insert(val).first;
    }
    SqlQuery& qry = iter->second;

    GroupIdCache::OptionalId grp_id;
    if (attr.parent_group())
        grp_id = group_id_cache_->get(*attr.parent_group());
    
    qry.binds().clear();
    qry.binds().add(":group_id", grp_id ? Variant(grp_id.get())
                                        : Variant());
    qry.binds().add(":attribute_id", Variant(mapping.id));
    qry.binds().add(":value", attr.value());
    
    return qry;
}

} // namespace rdb
} // namespace brfc
