/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <iostream>

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/test_common.hpp>

std::ostream& operator<<(std::ostream& out, const brfc::Date& value) {
    out << "Date("
        << value.year() << ", "
        << value.month() << ", "
        << value.day() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Time& value) {
    out << "Time("
        << value.hour() << ", "
        << value.minute() << ", "
        << value.second() << ", "
        << value.msec() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::DateTime& value) {
    out << "DateTime("
        << value.date().year() << ", "
        << value.date().month() << ", "
        << value.date().day() << ", "
        << value.time().hour() << ", "
        << value.time().minute() << ", "
        << value.time().second() << ", "
        << value.time().msec() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::TimeDelta& value) {
    out << "DateTime("
        << value.days() << ", "
        << value.msecs() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Variant& value) {
    out << value.to_string();
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::oh5::Scalar& value) {
    out << "oh5::Scalar("
        << value.to_string()
        << ")";
    return out;
}
