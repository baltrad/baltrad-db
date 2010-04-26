#ifndef BRFC_RDB_SAVE_ATTRIBUTE_HPP
#define BRFC_RDB_SAVE_ATTRIBUTE_HPP

#include <map>

#include <QtSql/QSqlQuery>

namespace brfc {

namespace oh5 {

class Attribute;

}

namespace rdb {

class AttributeMapper;
class GroupIdCache;
class RelationalDatabase;

class SaveAttribute {
  public:
    SaveAttribute(RelationalDatabase* rdb, GroupIdCache* group_id_cache);

    void operator()(const oh5::Attribute& attr);

  private:
    typedef std::map<QString, QSqlQuery> QueryMap;

    QSqlQuery& invalid_attribute_query(const oh5::Attribute& attr);
   
    QSqlQuery& valid_attribute_query(const oh5::Attribute& attr);

    RelationalDatabase* rdb_;
    const AttributeMapper* mapper_;
    GroupIdCache* group_id_cache_;
    QueryMap queries_;
};



} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SAVE_ATTRIBUTE_HPP
