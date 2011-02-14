/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/sql/pg/Connection.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <pqxx/connection>
#include <pqxx/largeobject>
#include <pqxx/transaction>

#include <brfc/exceptions.hpp>
#include <brfc/Url.hpp>
#include <brfc/sql/pg/Result.hpp>

namespace brfc {
namespace sql {
namespace pg {

Connection::Connection(const Url& url)
        : conn_()
        , transaction_()
        , dialect_()
        , compiler_(&dialect_) {
    try {
        conn_.reset(new pqxx::connection(url_to_pg(url)));
    } catch (const pqxx::broken_connection& e) {
        throw db_error(e.what());
    }
    conn_->set_client_encoding("utf8");
    conn_->set_variable("datestyle", "ISO");
    load_type_oids();
    dialect_.version(conn_->server_version());
    dialect_.conn(conn_.get());
}

Connection::Connection(pqxx::connection_base* conn)
        : conn_(conn, no_delete)
        , transaction_()
        , dialect_()
        , compiler_(&dialect_) {

}

Connection::~Connection() {

}

void
Connection::pqxx_transaction(pqxx::dbtransaction* tx) {
    transaction_.reset(tx, no_delete);
}

bool
Connection::do_is_open() const {
    if (conn_ and conn_->is_open())
        return true;
    return false;
}

void
Connection::do_close() {
    conn_.reset();
}

void
Connection::do_begin() {
    transaction_.reset(new pqxx::transaction<>(*conn_));
}

void
Connection::do_commit() {
    transaction_->commit();
    transaction_.reset();
}

void
Connection::do_rollback() {
    transaction_.reset();
}

sql::Result*
Connection::do_execute(const std::string& query) {
    auto_ptr<sql::Result> result;
    try {
        pqxx::result pg_result = transaction_->exec(query);
        result.reset(new Result(pg_result, &types_));
    } catch (const pqxx::broken_connection& e) {
        conn_.reset();
        throw db_error(e.what());
    } catch (const pqxx::failure& e) {
        throw db_error(e.what());
    }

    return result.release();
}

std::string
Connection::url_to_pg(const Url& url) {
    std::vector<std::string> pgargs;
    if (url.host() != "")
        pgargs.push_back("host=" + url.host());
    if (url.user_name() != "")
        pgargs.push_back("user=" + url.user_name());
    if (url.password() != "")
        pgargs.push_back("password=" + url.password());
    if (url.port())
        pgargs.push_back("port=" + boost::lexical_cast<std::string>(url.port()));
    const std::string& database = url.http_path();
    if (database != "")
        pgargs.push_back("dbname=" + database);

    return boost::join(pgargs, " ");
}

long long
Connection::do_store_large_object(const std::string& path) {
    pqxx::largeobject lobj(*transaction_, path);
    return boost::numeric_cast<long long >(lobj.id());
}

void
Connection::do_large_object_to_file(long long id,
                                    const std::string& path) {
    pqxx::largeobject lobj(boost::numeric_cast<pqxx::oid>(id));
    lobj.to_file(*transaction_, path);
}

void
Connection::load_type_oids() {
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

} // namespace pg
} // namespace sql
} // namespace brfc
