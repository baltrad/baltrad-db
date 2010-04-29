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

#include <QtCore/QString>
#include <QtCore/QRegExp>

#include <brfc/exceptions.hpp>

#include <brfc/rdb/SqlQuery.hpp>

namespace brfc {
namespace rdb {

void
Connection::begin() {
    if (in_transaction())
        throw db_error("transaction already active");
    do_begin();
}

void
Connection::rollback() {
    if (not in_transaction())
        throw db_error("no active transaction");
    do_rollback();
}

void
Connection::commit() {
    if (not in_transaction())
        throw db_error("no active transaction");
    do_commit();
}

shared_ptr<ResultSet>
Connection::execute(const QString& statement_, const BindMap& binds) {
    QString statement(statement_);
    if (binds.size() > 0) {
        statement = replace_binds(statement, binds);
    }
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

shared_ptr<ResultSet>
Connection::execute(const SqlQuery& query) {
    return execute(query.statement(), query.binds());
}

QString
Connection::replace_binds(const QString& statement_, const BindMap& binds) {
    QRegExp bind_re(":[a-zA-Z0-9_]+");
    QString statement(statement_), bind_str;
    int pos = 0, ppos = 0;
    size_t replace_count = 0;
    while (bind_re.indexIn(statement, pos) != -1) {
        ppos = bind_re.pos() ? bind_re.pos() - 1 : 0;
        if (statement.at(ppos) == '\\') {
            pos = bind_re.pos() + bind_re.matchedLength();
            continue;
        }
        try {
            bind_str = variant_to_string(binds.get(bind_re.cap()));
        } catch (const lookup_error&) {
            throw value_error("missing value for bind placeholder " +
                              bind_re.cap().toStdString());
        }
        statement.replace(bind_re.pos(), bind_re.matchedLength(), bind_str);
        pos = bind_re.pos() + bind_str.length();
        ++replace_count;
    }

    // XXX: should there be a flag to omit this test?
    if (replace_count != binds.size())
        throw value_error("not all binds consumed");
    return statement;
}

QString
Connection::do_variant_to_string(const Variant& value) {
    switch (value.type()) {
        case Variant::NONE:
            return "NULL";
        case Variant::STRING:
            // XXX: escape!!!
            return "'" + value.string() + "'";
        case Variant::LONGLONG:
        case Variant::DOUBLE:
            return value.to_string();
        case Variant::BOOL:
            return value.to_string().toLower();
        case Variant::DATE:
            return "'" + value.date().toString("yyyy-MM-dd") + "'";
        case Variant::TIME:
            return "'" + value.time().toString("HH:mm:ss.zzz") + "'";
        default:
            throw value_error(value.to_string().toStdString());
    }
}

} // namespace rdb
} // namespace brfc
