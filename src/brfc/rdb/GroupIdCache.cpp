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

#include <brfc/rdb/GroupIdCache.hpp>

#include <brfc/oh5/Group.hpp>

#include <brfc/rdb/BindMap.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/rdb/RelationalResultSet.hpp>

namespace brfc {
namespace rdb {

GroupIdCache::GroupIdCache(RelationalDatabase* rdb)
        : IdCache<oh5::Group, long long>() 
        , rdb_(rdb) {

}

GroupIdCache::OptionalId
GroupIdCache::do_query(const oh5::Group& group) {
    QString qry = "SELECT id FROM groups WHERE file_id = :file_id "
                  "AND parent_id = :parent_id AND name = :name ";
    BindMap binds;
    binds.add(":file_id", rdb_->db_id(*group.file()));
    shared_ptr<const oh5::Group> parent = group.parent<const oh5::Group>();

    QVariant parent_id;
    if (parent) {
        OptionalId id = get(*parent);
        if (id)
            parent_id = id.get();
    }
    binds.add(":parent_id", parent_id);
    binds.add(":name", group.name());
    shared_ptr<ResultSet> r = rdb_->query(qry, binds);
    return r->next() ? r->integer(0) : OptionalId();
}

} // namespace rdb
} // namespace brfc