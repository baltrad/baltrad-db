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

/**
 * @brief save oh5::Attribute instances to database
 */
class SaveAttribute {
  public:
    /**
     * @brief constrcutor
     * @param rdb database to save to
     * @param group_id_cache cache of group ids
     */
    SaveAttribute(RelationalDatabase* rdb, GroupIdCache* group_id_cache);
    
    /**
     * @brief save an oh5::Attribute instance to database
     *
     * Invalid attributes are inserted to dbd_invalid_attributes,
     * specialized attributes are ignored. All other attributes are saved
     * to the table they are mapped to.
     */
    void operator()(const oh5::Attribute& attr);

  private:
    /**
     * @brief map table name to a QSqlQuery
     */
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
