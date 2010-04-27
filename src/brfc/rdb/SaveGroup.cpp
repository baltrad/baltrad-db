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

#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QtSql/QSqlError>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/GroupIdCache.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/rdb/RelationalResultSet.hpp>


namespace brfc {
namespace rdb {

SaveGroup::SaveGroup(RelationalDatabase* rdb,
                     GroupIdCache* id_cache)
        : rdb_(rdb)
        , id_cache_(id_cache)
        , qry_(rdb->connection())
        , special_(rdb->mapper().specializations_on("bdb_groups")) {
    QStringList columns, binds; 
    columns.append("parent_id");
    columns.append("file_id");
    columns.append("name");
    BOOST_FOREACH(const Mapping& mapping, special_) {
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const QString& column, columns) {
        binds.append(":" + column);
    }
    QString qrystr("INSERT INTO bdb_groups(" + columns.join(", ") +
                   ") VALUES(" + binds.join(", ") + ")");
    if (rdb->supports_returning())
        qrystr += " RETURNING id";
    if (not qry_.prepare(qrystr))
        throw db_error(qry_.lastError());
}

void
SaveGroup::operator()(const oh5::Group& group) {
    bind_plain(group);
    bind_specializations(group);

    if (not qry_.exec())
        throw db_error(qry_.lastError());
    id_cache_->set_cached(group.shared_from_this(), last_id());
}


void
SaveGroup::bind_plain(const oh5::Group& group) {
    QVariant file_id;
    GroupIdCache::OptionalId parent_id;
    shared_ptr<const oh5::Group> parent = group.parent<oh5::Group>();
    if (parent) {
        parent_id = id_cache_->get(*parent);
    }
    if (group.file())
        file_id = rdb_->db_id(*group.file());

    qry_.bindValue(":parent_id", parent_id ? parent_id.get()
                                           : QVariant());
    qry_.bindValue(":file_id", file_id);
    qry_.bindValue(":name", group.name());
}

void
SaveGroup::bind_specializations(const oh5::Group& group) {
    BOOST_FOREACH(const Mapping& mapping, special_) {
        QVariant val;
        shared_ptr<const oh5::Attribute> attr =
            group.child_attribute(mapping.attribute);
        if (attr)
            val = attr->value().to_qvariant();
        qry_.bindValue(":" + mapping.column, val);
    }
}

long long
SaveGroup::last_id() {
    if (rdb_->supports_returning()) {
        qry_.first();
        return qry_.value(0).toLongLong();
    } else {
        return qry_.lastInsertId().toLongLong();
    }
}

} // namespace rdb
} // namespace brfc
