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

#include <brfc/rdb/GroupCache.hpp>

#include <brfc/ResultSet.hpp>

#include <brfc/rdb/BindMap.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace rdb {

GroupCache::GroupCache(RelationalDatabase* rdb)
        : Cache<long long, weak_ptr<const oh5::Group> >()
        , rdb_(rdb) {

}

GroupCache::OptionalKey
GroupCache::do_lookup_key(weak_ptr<const oh5::Group> weak_group) {
    shared_ptr<const oh5::Group> group = weak_group.lock();
    String qry = "SELECT id FROM groups WHERE file_id = :file_id "
                  "AND parent_id = :parent_id AND name = :name ";
    BindMap binds;
    binds.add(":file_id", Variant(rdb_->db_id(*group->file())));
    shared_ptr<const oh5::Group> parent = group->parent<const oh5::Group>();

    Variant parent_id;
    if (parent) {
        OptionalKey id = key(parent);
        if (id)
            parent_id = Variant(id.get());
    }
    binds.add(":parent_id", parent_id);
    binds.add(":name", Variant(group->name()));
    shared_ptr<ResultSet> r = rdb_->query(qry, binds);
    return r->next() ? r->int64_(0) : OptionalKey();
}

} // namespace rdb
} // namespace brfc