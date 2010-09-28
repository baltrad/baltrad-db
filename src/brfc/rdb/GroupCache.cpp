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

#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>

#include <brfc/sql/expr.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Result.hpp>

namespace brfc {
namespace rdb {

GroupCache::GroupCache(RelationalDatabase* rdb)
        : Cache<long long, const oh5::Group*>()
        , rdb_(rdb) {

}

GroupCache::OptionalKey
GroupCache::do_lookup_key(const oh5::Group* group) {
    const Model& m = Model::instance();
    sql::Factory xpr;

    sql::SelectPtr qry = sql::Select::create();
    qry->what(m.groups->column("id"));

    qry->where(m.groups->column("name")->eq(xpr.string(group->name())));
    
    long long file_id = rdb_->db_id(*group->file());
    if (file_id)
        qry->where(qry->where()->and_(m.groups->column("file_id")->eq(xpr.int64_(file_id))));

    const oh5::Group* parent = group->parent<const oh5::Group>();
    if (parent) {
        OptionalKey id = key(parent);
        if (id)
            qry->where(qry->where()->and_(m.groups->column("parent_id")->eq(xpr.int64_(id.get()))));
    }
    shared_ptr<sql::Result> r = rdb_->connection().execute(*qry);
    return r->next() ? r->value_at(0).int64_() : OptionalKey();
}

} // namespace rdb
} // namespace brfc
