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

#include <brfc/rdb/SqlQuery.hpp>

namespace brfc {
namespace rdb {

QString
SqlQuery::replace_binds(const Connection& connection) const {
    if (binds_.size() == 0)
        return statement_;
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
            bind_str = connection.variant_to_string(binds_.get(bind_re.cap()));
        } catch (const lookup_error&) {
            throw value_error("missing value for bind placeholder " +
                              bind_re.cap().toStdString());
        }
        statement.replace(bind_re.pos(), bind_re.matchedLength(), bind_str);
        pos = bind_re.pos() + bind_str.length();
        ++replace_count;
    }

    // XXX: should there be a flag to omit this test?
    if (replace_count != binds_.size())
        throw value_error("not all binds consumed");
    return statement;
}


} // namespace rdb
} // namespace brfc
