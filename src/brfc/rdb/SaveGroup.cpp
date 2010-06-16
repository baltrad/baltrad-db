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
#include <brfc/ResultSet.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/GroupCache.hpp>
#include <brfc/rdb/Connection.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>


namespace brfc {
namespace rdb {

SaveGroup::SaveGroup(RelationalDatabase* rdb,
                     GroupCache* cache)
        : rdb_(rdb)
        , cache_(cache)
        , qry_("")
        , special_(rdb->mapper()->specializations_on("bdb_groups")) {
    StringList columns, binds; 
    columns.append("parent_id");
    columns.append("file_id");
    columns.append("name");
    BOOST_FOREACH(const Mapping& mapping, special_) {
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const String& column, columns) {
        binds.append(":" + column);
    }
    String qrystr("INSERT INTO bdb_groups(" + columns.join(", ") +
                   ") VALUES(" + binds.join(", ") + ")");
    if (rdb->connection().has_feature(Connection::RETURNING))
        qrystr += " RETURNING id";
    qry_.statement(qrystr);
}

void
SaveGroup::operator()(const oh5::Group& group) {
    qry_.binds().clear();
    bind_plain(group);
    bind_specializations(group);

    shared_ptr<ResultSet> result = rdb_->connection().execute(qry_);

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

    qry_.binds().add(":parent_id", parent_id ? Variant(parent_id.get())
                                             : Variant());
    qry_.binds().add(":file_id", file_id);
    qry_.binds().add(":name", Variant(group.name()));
}

void
SaveGroup::bind_specializations(const oh5::Group& group) {
    BOOST_FOREACH(const Mapping& mapping, special_) {
        shared_ptr<const oh5::Attribute> attr =
            group.child_attribute(mapping.attribute);
        qry_.binds().add(":" + mapping.column, attr ? attr->value()
                                                    : Variant());
    }
    // XXX: note that this relies on implicit convertion of attribute value
    //      in DB (True/False -> bool; ISO 8601 date/time strings)
    //
    // XXX: this should be explicit
}

long long
SaveGroup::last_id(ResultSet& result) {
    if (rdb_->connection().has_feature(Connection::RETURNING) and result.next()) {
        return result.int64_(0);
    } else {
        // XXX: last insert id!
        return 0;
    }
}

} // namespace rdb
} // namespace brfc
