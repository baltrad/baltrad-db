#ifndef BRFC_TEST_RDB_HPP
#define BRFC_TEST_RDB_HPP

#include <brfc/RelationalDatabase.hpp>

#include <QtCore/QDir>

class QStringList;

namespace brfc {

/**
 * @brief a helper class for tests
 */
class TestRDB : public RelationalDatabase {
  public:
    TestRDB(const std::string& dsn, const std::string& schema_dir);

    void create();
    void drop();
    void clean();

  private:
    QStringList load_queries(const QString& filename);
    void exec_queries_from(const QString& filename);

    QDir schema_dir_;
};

} // namespace brfc

#endif // BRFC_TEST_RDB_HPP 
