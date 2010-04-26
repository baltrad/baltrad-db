#ifndef BRFC_RDB_GROUP_ID_CACHE_HPP
#define BRFC_RDB_GROUP_ID_CACHE_HPP

#include <brfc/rdb/IdCache.hpp>

namespace brfc {

namespace oh5 {

class Group;

}

namespace rdb {

class RelationalDatabase;

/**
 * @brief map oh5::Group instances to their database ids
 */
class GroupIdCache : public IdCache<oh5::Group, long long> {
  public:
    /**
     * @brief constructor
     *
     * @param rdb database this cache operates on
     */
    GroupIdCache(RelationalDatabase* rdb);

  protected:
    /**
     * @brief query for existing id from database
     */
    virtual OptionalId do_query(const oh5::Group& group);   

  private:
    RelationalDatabase* rdb_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_GROUP_ID_CACHE_HPP
