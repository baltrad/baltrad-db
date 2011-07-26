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

#include <brfc/Date.hpp>

#include <boost/lexical_cast.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/TimeDelta.hpp>
#include <brfc/util/iso8601.hpp>

namespace brfc {

static int _days_in_months[] = {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

Date::Date(int year, int month, int day)
        : jdn_(jdn_from_date(year, month, day)) {
    if (year < 1)
        throw std::invalid_argument("year out of range");
    if (month <= 0 or month > 12)
        throw std::invalid_argument("month out of range");
    if (day <= 0 or day > _days_in_months[month - 1])
        throw std::invalid_argument("day out of range");
    if (not is_leap_year(year) and month == 2 and day > 28)
        throw std::invalid_argument("day out of range");
}

Date::Date(unsigned int jdn)
        : jdn_(jdn) {
}

unsigned int
Date::jdn_from_date(int year,
                    int month,
                    int day) {
    return ( 1461 * ( year + 4800 + ( month - 14 ) / 12 ) ) / 4 +
           ( 367 * ( month - 2 - 12 * ( ( month - 14 ) / 12 ) ) ) / 12 -
           ( 3 * ( ( year + 4900 + ( month - 14 ) / 12 ) / 100 ) ) / 4 +
           day - 32075;
}

void
Date::date_from_jdn(unsigned int jdn,
                    int& year,
                    int& month,
                    int& day) {
    long long l, n, i, j;
    l = jdn + 68569;
    n = ( 4 * l ) / 146097;
    l = l - ( 146097 * n + 3 ) / 4;
    i = ( 4000 * ( l + 1 ) ) / 1461001,
    l = l - ( 1461 * i ) / 4 + 31;
    j = ( 80 * l ) / 2447;
    day = l - ( 2447 * j ) / 80;
    l = j / 11;
    month = j + 2 - ( 12 * l );
    year = 100 * ( n - 49 ) + i + l;
}

bool
Date::is_leap_year(int year) {
    return ((year % 4) == 0 and (year % 100) != 0) or (year % 400) == 0;
}

int
Date::days_in_month(int year, int month) {
    if (month < 1 or month > 12)
        throw std::invalid_argument("month out of range: "
                          + boost::lexical_cast<std::string>(month));
    if (month == 2 and not is_leap_year(year)) {
        return 28;
    } else {
        return _days_in_months[month - 1];
    }
}

Date
Date::today() {
    return DateTime::now().date();
}

Date
Date::utc_today() {
    return DateTime::utc_now().date();
}

int
Date::year() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return year;
}

int
Date::month() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return month;
}

int
Date::day() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return day;
}

Date&
Date::year(int year) {
    int _, month, day;
    date_from_jdn(jdn_, _, month, day);
    if (days_in_month(year, month) < day)
        throw std::invalid_argument("setting year would result in invalid date");
    jdn_ = jdn_from_date(year, month, day);
    return *this;
}

Date&
Date::month(int month) {
    int year, _, day;
    date_from_jdn(jdn_, year, _, day);
    if (days_in_month(year, month) < day)
        throw std::invalid_argument("setting month would result in invalid date");
    jdn_ = jdn_from_date(year, month, day);
    return *this;
}

Date&
Date::day(int day) {
    int year, month, _;
    date_from_jdn(jdn_, year, month, _);
    if (day < 1 or days_in_month(year, month) < day)
        throw std::invalid_argument("setting month would result in invalid date");
    jdn_ = jdn_from_date(year, month, day);
    return *this;
}

Date&
Date::operator+=(const TimeDelta& td) {
    jdn_ += td.days();
    return *this;
}

Date
Date::operator+(const TimeDelta& td) const {
    return Date(*this) += td;
}

Date
Date::from_iso_string(const std::string& str) {
    iso8601::date d;
    if (not iso8601::parse_date(str, d))
        throw std::invalid_argument("invalid ISO8601 date: " + str);
    return Date(d.year, d.month, d.day);
}

std::string
Date::to_iso_string(bool extended) const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    iso8601::date d(year, month, day);
    if (extended)
        return iso8601::to_string(d, iso8601::format::EXTENDED);
    else
        return iso8601::to_string(d, iso8601::format::BASIC);
}

} // namespace brfc
