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

#include <brfc/sql/Connection.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/String.hpp>
#include <brfc/Url.hpp>

#include <brfc/sql/DefaultCompiler.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/pg/Connection.hpp>

namespace brfc {
namespace sql {

Connection::~Connection() {

}

shared_ptr<Connection>
Connection::create(const String& dsn) {
    Url url(dsn);
    if (url.scheme() == "postgresql")
        return make_shared<pg::Connection>(url);
    else
        throw value_error("no mapping found for dsn scheme: "
                          + url.scheme().to_utf8());
}

void
Connection::open() {
    if (is_open())
        throw db_error("connection already open");
    do_open();
}

void
Connection::close() {
    if (not is_open())
        throw db_error("connection already closed");
    do_close();
}

void
Connection::begin() {
    if (not is_open())
        throw db_error("no open connection");
    if (in_transaction())
        throw db_error("transaction already active");
    do_begin();
}

void
Connection::rollback() {
    if (not is_open())
        throw db_error("no open connection");
    if (not in_transaction())
        throw db_error("no active transaction");
    do_rollback();
}

void
Connection::commit() {
    if (not is_open())
        throw db_error("no open connection");
    if (not in_transaction())
        throw db_error("no active transaction");
    do_commit();
}

shared_ptr<Result>
Connection::execute(const String& statement, const BindMap& binds) {
    return execute(Query(statement, binds));    
}

shared_ptr<Result>
Connection::execute(const Insert& stmt) {
    return execute(compiler().compile(stmt));
}

shared_ptr<Result>
Connection::execute(const Select& stmt) {
    return execute(compiler().compile(stmt));
}

shared_ptr<Result>
Connection::execute(const Query& query) {
    String statement = query.replace_binds(dialect());
    shared_ptr<Result> result;
    if (not in_transaction()) {
        try {
            begin();
            result = do_execute(statement);
            commit();
        } catch (const std::runtime_error& e) {
            rollback();
            throw db_error(e.what());
        }
    } else {
        try {
            result = do_execute(statement);
        } catch (const std::runtime_error& e) {
            throw db_error(e.what());
        }
    }
    return result;
}

shared_ptr<LargeObject>
Connection::large_object(long long id) {
    return do_large_object(id);
}

shared_ptr<LargeObject>
Connection::large_object(const String& path) {
    return do_large_object(path);
}

} // namespace sql
} // namespace brfc
