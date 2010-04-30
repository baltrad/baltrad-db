#include <brfc/rdb/PostgresConnection.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/rdb/PostgresResultSet.hpp>

namespace brfc {
namespace rdb {



PostgresConnection::PostgresConnection(const QUrl& url)
        : url_(url)
        , conn_()
        , transaction_() {
}

PostgresConnection::~PostgresConnection() {

}

void
PostgresConnection::do_open() {
    conn_.reset(new pqxx::connection(url_to_pg(url_).toStdString()));
    conn_->set_client_encoding("utf8");
    conn_->set_variable("datestyle", "ISO");
}

bool
PostgresConnection::do_is_open() const {
    if (conn_ and conn_->is_open())
        return true;
    return false;
}

void
PostgresConnection::do_close() {
    conn_.reset();
}

void
PostgresConnection::do_begin() {
    transaction_.reset(new pqxx::transaction<>(*conn_));
}

void
PostgresConnection::do_commit() {
    transaction_->commit();
    transaction_.reset();
}

void
PostgresConnection::do_rollback() {
    transaction_.reset();
}

shared_ptr<ResultSet>
PostgresConnection::do_execute(const QString& query) {
    shared_ptr<ResultSet> result;
    try {
        pqxx::result pg_result = transaction_->exec(query.toUtf8().constData());
        result = make_shared<PostgresResultSet>(pg_result);
    } catch (const std::runtime_error& e) {
        throw db_error(e.what());
    }

    return result;
}

QString
PostgresConnection::url_to_pg(const QUrl& url) {
    QString pgargs;
    if (url.host() != "")
        pgargs += " host=" + url.host();
    if (url.userName() != "")
        pgargs += " user=" + url.userName();
    if (url.password() != "")
        pgargs += " password=" + url.password();
    if (url.port() != -1)
        pgargs += " port=" + QString::number(url.port());
    QString database = url.path();
    if (database.startsWith("/")) {
        database.remove(0, 1); // remove slash
    }
    if (database != "")
        pgargs += " dbname=" + database;

    return pgargs;
}

bool
PostgresConnection::do_has_feature(Connection::Feature feature) const {
    switch (feature) {
        case RETURNING:
            // XXX: supported since 8.2, do a version check
            return true;
        case LAST_INSERT_ID:
            // technically possible, but useless
            // if database supports OIDs, last OID can be fetched
            return false;
        default:
            return false;
    }
}

} // namespace rdb
} // namespace brfc
