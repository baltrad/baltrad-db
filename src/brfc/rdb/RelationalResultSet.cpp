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

#include <brfc/rdb/RelationalResultSet.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <QtCore/QVariant>
#include <QtSql/QSqlQuery>

namespace brfc {
namespace rdb {

RelationalResultSet::RelationalResultSet(const QSqlQuery& query,
                                         shared_ptr<QSqlDatabase> db)
        : db_(db)
        , query_(new QSqlQuery(query)) {
    query_->seek(-1);
}

RelationalResultSet::~RelationalResultSet() {

}

bool
RelationalResultSet::do_next() {
    return query_->next();
}

bool
RelationalResultSet::do_seek(int idx) {
    return query_->seek(idx);
}

int
RelationalResultSet::do_size() {
    return query_->size();
}

Variant
RelationalResultSet::do_value_at(unsigned int pos) const {
    const QVariant& var = query_->value(pos);
    if (not var.isValid()) {
        throw lookup_error("invalid position");
    }
    return Variant(var);
}

} // namespace rdb
} // namespace brfc
