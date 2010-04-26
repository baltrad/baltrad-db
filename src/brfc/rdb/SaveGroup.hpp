#ifndef BRFC_RDB_SAVE_GROUP_HPP
#define BRFC_RDB_SAVE_GROUP_HPP

#include <QtSql/QSqlQuery>

#include <brfc/rdb/AttributeMapper.hpp>

namespace brfc {

namespace oh5 {

class Group;

}

namespace rdb {

class RelationalDatabase;
class GroupIdCache;

/**
 * @brief save oh5::Group instances to database
 */
class SaveGroup {
  public:
    /**
     * @brief constructor
     *
     * @param rdb database to save to
     * @param id_cache cache for inserted group ids
     * @throw db_error if preparing the query fails
     *
     * prepare a query for use by operator(). If the database supports
     * RETURNING, use it to fetch the id of inserted group entry.
     */
    SaveGroup(RelationalDatabase* rdb, GroupIdCache* id_cache);
    
    /**
     * @brief save a oh5::Group instance to database
     * @throw db_error if executing the query fails
     * 
     * bind columns to the prepared query and execute it. If successful,
     * add group id to cache.
     */
    void operator()(const oh5::Group& group);

  private:
    /**
     * @brief bind non-attribute columns
     *
     * bind parent group's id, associated file's id and group name
     * to the prepared query.
     */
    void bind_plain(const oh5::Group& group);
    
    /**
     * @brief bind attributes specialized on this table
     */
    void bind_specializations(const oh5::Group& group);
    
    /**
     * @brief id of the last insert
     */
    long long last_id();

    RelationalDatabase* rdb_;
    GroupIdCache* id_cache_;
    QSqlQuery qry_;
    MappingVector special_;
};



} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SAVE_GROUP_HPP
