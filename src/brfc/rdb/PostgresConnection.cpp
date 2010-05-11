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
    conn_.reset(new pqxx::connection(url_to_pg(url_).to_std_string()));
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
PostgresConnection::do_execute(const String& query) {
    shared_ptr<ResultSet> result;
    try {
        pqxx::result pg_result = transaction_->exec(query.to_utf8());
        result = make_shared<PostgresResultSet>(pg_result);
    } catch (const std::runtime_error& e) {
        throw db_error(e.what());
    }

    return result;
}

String
PostgresConnection::url_to_pg(const QUrl& url) {
    String pgargs;
    if (url.host() != "")
        pgargs += " host=" + String::from_utf16(url.host().utf16());
    if (url.userName() != "")
        pgargs += " user=" + String::from_utf16(url.userName().utf16());
    if (url.password() != "")
        pgargs += " password=" + String::from_utf16(url.password().utf16());
    if (url.port() != -1)
        pgargs += " port=" + String::number(url.port());
    String database = String::from_utf16(url.path().utf16());
    if (database.starts_with("/")) {
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