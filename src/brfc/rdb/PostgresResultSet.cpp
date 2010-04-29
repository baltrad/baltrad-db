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

#include <QtCore/Qt>
#include <brfc/rdb/PostgresResultSet.hpp>

#include <brfc/Variant.hpp>

namespace brfc {
namespace rdb {

PostgresResultSet::PostgresResultSet(const pqxx::result& result)
        : result_(result)
        , row_(-1) {

}

bool
PostgresResultSet::do_next() {
    ++row_;
    return row_ < size();
}

bool
PostgresResultSet::do_seek(int idx) {
    row_ += idx;
    return row_ < size() and row_ > -1;
}

int
PostgresResultSet::do_size() {
    return result_.size();
}

bool
PostgresResultSet::do_is_null(unsigned int pos) const {
    return result_.at(row_).at(pos).is_null();
}

QString
PostgresResultSet::do_string(unsigned int pos) const {
    return QString::fromUtf8(result_.at(row_).at(pos).c_str());
}

long long
PostgresResultSet::do_integer(unsigned int pos) const {
    long long val;
    result_.at(row_).at(pos).to(val);
    return val;
}

double
PostgresResultSet::do_real(unsigned int pos) const {
    double val;
    result_.at(row_).at(pos).to(val);
    return val;
}

bool
PostgresResultSet::do_boolean(unsigned int pos) const {
    bool val;
    result_.at(row_).at(pos).to(val);
    return val;
}

QDate
PostgresResultSet::do_date(unsigned int pos) const {
    QString str = QString::fromUtf8(result_.at(row_).at(pos).c_str());
    return QDate::fromString(str, Qt::ISODate);
}

QTime
PostgresResultSet::do_time(unsigned int pos) const {
    QString str = QString::fromUtf8(result_.at(row_).at(pos).c_str());
    QTime time = QTime::fromString(str, "hh:mm:ss.zzz");
    if (not time.isValid()) {
        time = QTime::fromString(str, "h:mm:ss");
    }
    return time;
}

} // namespace rdb
} // namespace brfc
