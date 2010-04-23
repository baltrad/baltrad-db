#ifndef BRFC_RDB_GROUP_ID_CACHE_HPP
#define BRFC_RDB_GROUP_ID_CACHE_HPP

#include <brfc/rdb/IdCache.hpp>

namespace brfc {

namespace oh5 {

class Group;

}

namespace rdb {

class RelationalDatabase;

class GroupIdCache : public IdCache<oh5::Group, long long> {
  public:
    GroupIdCache(RelationalDatabase* rdb);

  protected:
    virtual OptionalId do_query(const oh5::Group& group);   

  private:
    RelationalDatabase* rdb_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_GROUP_ID_CACHE_HPP
