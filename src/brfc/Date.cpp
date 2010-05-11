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

#include <brfc/Date.hpp>

#include <cmath>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include <QtCore/QDate>

#include <brfc/exceptions.hpp>
#include <brfc/String.hpp>

namespace brfc {

static unsigned int _days_in_months[] = {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

Date::Date(int year, unsigned int month, unsigned int day)
        : jdn_(jdn_from_date(year, month, day)) {
    if (year < 1)
        throw value_error("year out of range");
    if (month == 0 or month > 12)
        throw value_error("month out of range");
    if (day == 0 or day > _days_in_months[month - 1])
        throw value_error("day out of range");
    if (not is_leap_year(year) and month == 2 and day > 28)
        throw value_error("day out of range");
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

unsigned int
Date::days_in_month(int year, int month) {
    if (month < 1 or month > 12)
        throw value_error("month out of range: "
                          + boost::lexical_cast<std::string>(month));
    if (month == 2 and not is_leap_year(year)) {
        return 28;
    } else {
        return _days_in_months[month - 1];
    }
}

Date
Date::today() {
    return Date(1, 1, 1);
}

Date
Date::utc_today() {
    return Date(1, 1, 1);
}

int
Date::year() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return year;
}

unsigned int
Date::month() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return month;
}

unsigned int
Date::day() const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    return day;
}

Date
Date::add_years(int years) const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);
    year += years;
    day = std::min(day, (int)days_in_month(year, month));
    return Date(year, month, day);
}

Date
Date::add_months(int months) const {
    int year, month, day;
    date_from_jdn(jdn_, year, month, day);

    bool negative = months < 0;
    int ayears = std::abs(months) / 12;
    int amonths = std::abs(months) % 12;
    
    year += negative ? -ayears : ayears;

    if (negative) {
        month -= amonths;
        // underflow
        if (month <= 0) {
            year--;
            month = 12 + month;
        }
    } else {
        month += amonths;
        // overflow
        if (month >= 13) {
            year++;
            month -= 12;
        }
    }
    day = std::min(day, (int)days_in_month(year, month));
    return Date(year, month, day);
}

Date
Date::add_days(int days) const {
    return Date(jdn_ + days);
}

Date
Date::from_string(const String& str, const String& format) {
    QString qstr = QString::fromUtf16(str.utf16());
    QString qformat = QString::fromUtf16(format.utf16());
    QDate d = QDate::fromString(qstr, qformat);
    if (not d.isValid())
        throw value_error("could not parse date from string");
    return Date(d.year(), d.month(), d.day());
}

String
Date::to_string(const String& format) const {
    QString qformat = QString::fromUtf16(format.utf16());
    QString qstr = QDate(year(), month(), day()).toString(qformat);
    return String::from_utf16(qstr.utf16());
}

} // namespace brfc