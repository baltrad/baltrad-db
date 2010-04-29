#ifndef BRFC_RDB_POSTGRES_CONNECTION_HPP
#define BRFC_RDB_POSTGRES_CONNECTION_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/rdb/Connection.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>

class QString;
class QUrl;

namespace brfc {
namespace rdb {

class BindMap;

class PostgresConnection : public Connection {
  public:
    PostgresConnection(const QUrl& url);

    static QString url_to_pg(const QUrl& url);
  
  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    virtual bool do_in_transaction() {
        return transaction_.get() != 0;
    }

    virtual shared_ptr<ResultSet> do_execute(const QString& query);

    virtual bool do_has_feature(Feature feature) const;

  private:
    pqxx::connection conn_;
    scoped_ptr<pqxx::transaction<> > transaction_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_POSTGRES_CONNECTION_HPP
