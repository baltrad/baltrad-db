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

class SaveGroup {
  public:
    SaveGroup(RelationalDatabase* rdb, GroupIdCache* id_cache);

    void operator()(const oh5::Group& group);

  private:
    void bind_plain(const oh5::Group& group);

    void bind_specializations(const oh5::Group& group);

    long long last_id();

    RelationalDatabase* rdb_;
    GroupIdCache* id_cache_;
    QSqlQuery qry_;
    MappingVector special_;
};



} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SAVE_GROUP_HPP
