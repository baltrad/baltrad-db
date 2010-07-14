#include <brfc/rdb/PostgresConnection.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/rdb/PostgresResultSet.hpp>

namespace brfc {
namespace rdb {

PostgresConnection::PostgresConnection(const Url& url)
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
    load_type_oids();
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
        result = make_shared<PostgresResultSet>(pg_result, &types_);
    } catch (const std::runtime_error& e) {
        throw db_error(e.what());
    }

    return result;
}

String
PostgresConnection::url_to_pg(const Url& url) {
    String pgargs;
    if (url.host() != "")
        pgargs += " host=" + url.host();
    if (url.user_name() != "")
        pgargs += " user=" + url.user_name();
    if (url.password() != "")
        pgargs += " password=" + url.password();
    if (url.port())
        pgargs += " port=" + String::number(url.port());
    String database = url.path();
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

void
PostgresConnection::load_type_oids() {
    const char* qry = "SELECT oid, typname FROM pg_catalog.pg_type "
                      "WHERE typname IN ('int2', 'int4', 'int8', "
                                        "'float4', 'float8', "
                                        "'bool', 'date', 'time')";
    pqxx::transaction<> tx(*conn_);
    pqxx::result result = tx.exec(qry);

    for (pqxx::result::size_type i = 0; i < result.size(); ++i) {
        pqxx::oid typoid = result.at(i).at(0).as<pqxx::oid>();
        std::string typname = result.at(i).at(1).as<std::string>();
        if (typname == "int2")
            types_.int2_oid = typoid;
        else if (typname == "int4")
            types_.int4_oid = typoid;
        else if (typname == "int8")
            types_.int8_oid = typoid;
        else if (typname == "float4")
            types_.float4_oid = typoid;
        else if (typname == "float8")
            types_.float8_oid = typoid;
        else if (typname == "bool")
            types_.bool_oid = typoid;
        else if (typname == "date")
            types_.date_oid = typoid;
        else if (typname == "time")
            types_.time_oid = typoid;
    }
}

} // namespace rdb
} // namespace brfc
