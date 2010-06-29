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

#include <brfc/DateTime.hpp>

#include <brfc/DateTimeParser.hpp>

#include <brfc/String.hpp>

namespace brfc {

String
DateTime::to_string(const String& format) const {
    return DateTimeParser(format).to_string(*this);
}

bool
DateTime::operator==(const DateTime& rhs) const {
    return date_ == rhs.date_ and time_ == rhs.time_;
}

DateTime
DateTime::from_string(const String& str, const String& format) {
    return DateTimeParser(format).from_string(str);
}

} // namespace brfc
