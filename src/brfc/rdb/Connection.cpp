#include <brfc/rdb/Connection.hpp>

#include <QtCore/QString>
#include <QtCore/QRegExp>

#include <brfc/exceptions.hpp>

#include <iostream>

namespace brfc {
namespace rdb {

shared_ptr<ResultSet>
Connection::execute(const QString& query, const BindMap& binds) {
    QString qry(query);
    if (binds.size() > 0) {
        qry = replace_binds(query, binds);
    }
    shared_ptr<ResultSet> result;
    if (not in_transaction()) {
        try {
            begin();
            result = do_execute(query);
            commit();
        } catch (...) {
            rollback();
            throw db_error("");
        }
    } else {
        try {
            result = do_execute(query);
        } catch (...) {
            throw db_error("");
        }
    }
    return result;
}

QString
Connection::replace_binds(const QString& query_, const BindMap& binds) {
    QRegExp bind_re(":[a-zA-Z0-9_]+");
    QString query(query_), bind_str;
    int pos = 0, ppos = 0;
    size_t replace_count = 0;
    while (bind_re.indexIn(query, pos) != -1) {
        ppos = bind_re.pos() ? bind_re.pos() - 1 : 0;
        if (query.at(ppos) == '\\') {
            pos = bind_re.pos() + bind_re.matchedLength();
            continue;
        }
        try {
            bind_str = variant_to_string(binds.get(bind_re.cap()));
        } catch (const lookup_error&) {
            throw value_error("missing value for bind placeholder " +
                              bind_re.cap().toStdString());
        }
        query.replace(bind_re.pos(), bind_re.matchedLength(), bind_str);
        pos += bind_str.length();
        ++replace_count;
    }

    // XXX: should there be a flag to omit this test?
    if (replace_count != binds.size())
        throw value_error("not all binds consumed");

    return query;
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
            return value.date().toString("yyyy-MM-dd");
        case Variant::TIME:
            return value.time().toString("HH:mm:ss.zzz");
        default:
            throw value_error(value.to_string().toStdString());
    }
}

} // namespace rdb
} // namespace brfc
