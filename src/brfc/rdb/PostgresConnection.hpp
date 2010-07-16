#ifndef BRFC_RDB_POSTGRES_CONNECTION_HPP
#define BRFC_RDB_POSTGRES_CONNECTION_HPP

#include <pqxx/connection>
#include <pqxx/transaction>

#include <brfc/smart_ptr.hpp>
#include <brfc/Url.hpp>
#include <brfc/rdb/Connection.hpp>

class String;

namespace brfc {
namespace rdb {

struct PgTypes {
    pqxx::oid int2_oid;
    pqxx::oid int4_oid;
    pqxx::oid int8_oid;
    pqxx::oid float4_oid;
    pqxx::oid float8_oid;
    pqxx::oid bool_oid;
    pqxx::oid date_oid;
    pqxx::oid time_oid;
};

class PostgresConnection : public Connection {
  public:
    explicit PostgresConnection(const Url& url);

    virtual ~PostgresConnection();

    static String url_to_pg(const Url& url);
  
  protected:
    virtual void do_open();
    virtual void do_close();
    virtual bool do_is_open() const;

    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    virtual bool do_in_transaction() const {
        return transaction_.get() != 0;
    }

    virtual String do_dialect() const {
        return "postgresql";
    }

    virtual shared_ptr<Result> do_execute(const String& query);

    virtual bool do_has_feature(Feature feature) const;

  private:
    void load_type_oids();

    Url url_;
    PgTypes types_;
    scoped_ptr<pqxx::connection> conn_;
    scoped_ptr<pqxx::transaction<> > transaction_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_POSTGRES_CONNECTION_HPP
