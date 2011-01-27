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

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <sys/time.h>

#include <brfc/exceptions.hpp>
#include <brfc/DateTimeParser.hpp>
#include <string>
#include <brfc/TimeDelta.hpp>

namespace brfc {

DateTime
DateTime::now() {
    struct timeval tv;
    if (gettimeofday(&tv, 0) == -1)
        throw std::runtime_error(strerror(errno));
    struct tm* tm;
    if ((tm = std::localtime(&tv.tv_sec)) == 0)
        throw std::runtime_error("std::localtime failed");
    Time t(tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
    Date d(1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday);
    return DateTime(d, t);
}

DateTime
DateTime::utc_now() {
    struct timeval tv;
    if (gettimeofday(&tv, 0) == -1)
        throw std::runtime_error(strerror(errno));
    struct tm* tm;
    if ((tm = std::gmtime(&tv.tv_sec)) == 0)
        throw std::runtime_error("std::gmtime failed");
    Time t(tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
    Date d(1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday);
    return DateTime(d, t);
}

std::string
DateTime::to_string(const std::string& format) const {
    return DateTimeParser(format).to_string(*this);
}

DateTime
DateTime::from_string(const std::string& str, const std::string& format) {
    return DateTimeParser(format).from_string(str);
}

struct tm
DateTime::to_tm() const {
    if (date().year() < 1900)
        throw value_error("can't convert DateTime to tm: year < 1900");
    struct tm t;
    t.tm_year = date().year() - 1900;
    t.tm_mon = date().month() - 1;
    t.tm_mday = date().day();
    t.tm_hour = time().hour();
    t.tm_min = time().minute();
    t.tm_sec = time().second();
    t.tm_wday = 0;
    t.tm_yday = 0;
    t.tm_isdst = 0;
    return t;
}

bool
DateTime::operator==(const DateTime& rhs) const {
    return date_ == rhs.date_ and time_ == rhs.time_;
}

bool
DateTime::operator<(const DateTime& rhs) const {
    if (date_ < rhs.date_)
        return true;
    if (date_ == rhs.date_ and time_ < rhs.time_)
        return true;
    return false;
}

DateTime&
DateTime::operator+=(const TimeDelta& td) {
    date_ += td;
    if (time_.cumulative_msecs() + td.msecs() >= MSECS_IN_DAY)
        date_ += TimeDelta().days(1);
    time_ += td;
    return *this;
}

DateTime
DateTime::operator+(const TimeDelta& td) const {
    return DateTime(*this) += td;
}

} // namespace brfc
