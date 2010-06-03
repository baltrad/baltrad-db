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

#include <brfc/rdb/Connection.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/String.hpp>

#include <brfc/rdb/SqlQuery.hpp>

namespace brfc {
namespace rdb {

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

shared_ptr<ResultSet>
Connection::execute(const String& statement, const BindMap& binds) {
    return execute(SqlQuery(statement, binds));    
}

shared_ptr<ResultSet>
Connection::execute(const SqlQuery& query) {
    String statement = query.replace_binds(*this);
    shared_ptr<ResultSet> result;
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

String
Connection::do_variant_to_string(const Variant& value) const {
    switch (value.type()) {
        case Variant::NONE:
            return "NULL";
        case Variant::STRING:
            // XXX: escape!!!
            return "'" + value.string() + "'";
        case Variant::INT64:
        case Variant::DOUBLE:
            return value.to_string();
        case Variant::BOOL:
            return value.to_string().to_lower();
        case Variant::DATE:
            return "'" + value.date().to_string("yyyy-MM-dd") + "'";
        case Variant::TIME:
            return "'" + value.time().to_string("HH:mm:ss.zzz") + "'";
        default:
            throw value_error(value.to_string().to_std_string());
    }
}

} // namespace rdb
} // namespace brfc
