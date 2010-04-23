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
