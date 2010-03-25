#ifndef BRFC_TEST_TEST_RDB_HPP
#define BRFC_TEST_TEST_RDB_HPP

#include <brfc/rdb/RelationalDatabase.hpp>

#include <QtCore/QDir>

class QStringList;

namespace brfc {
namespace test {

/**
 * @brief a helper class for tests
 *
 * add methods for creating/dropping database schema in a relational database
 */
class TestRDB : public rdb::RelationalDatabase {
  public:
    /**
     * @brief constructor
     *
     * @param dsn database sdn
     * @param schema_dir base directory for finding statements
     */
    TestRDB(const QString& dsn, const QString& schema_dir);
    
    /**
     * @brief execute create statements
     */
    void create();

    /**
     * @brief execute drop statements
     */
    void drop();

    /**
     * @brief clear database of all files
     */
    void clean();

  private:
    QStringList load_queries(const QString& filename);
    void exec_queries_from(const QString& filename);

    QDir schema_dir_;
};

} // namespace test
} // namespace brfc

#endif // BRFC_TEST_TEST_RDB_HPP 
