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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/ResultSet.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

QString
ResultSet::string(unsigned int pos) const {
    return value_at(pos).string();
}

long long
ResultSet::integer(unsigned int pos) const {
    return value_at(pos).longlong();
}

double
ResultSet::real(unsigned int pos) const {
    return value_at(pos).double_();
}

bool
ResultSet::boolean(unsigned int pos) const {
    return value_at(pos).bool_();
}

QDate
ResultSet::date(unsigned int pos) const {
    return value_at(pos).date();
}

QTime
ResultSet::time(unsigned int pos) const {
    return value_at(pos).time();
}

Variant
ResultSet::value(unsigned int pos) const {
    return value_at(pos);
}

Variant
ResultSet::value_at(unsigned int pos) const {
    return do_value_at(pos);
}

} // namespace brfc
