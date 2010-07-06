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

#include <brfc/Time.hpp>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <sys/time.h>

#include <brfc/exceptions.hpp>
#include <brfc/DateTimeParser.hpp>
#include <brfc/String.hpp>

namespace {
    enum {
        HOURS_IN_DAY = 24,
        MINS_IN_HOUR = 60,
        SECS_IN_MIN = 60,
        MSECS_IN_SEC = 1000,
        MSECS_IN_MIN = 60000,
        MSECS_IN_HOUR = 3600000,
        MSECS_IN_DAY = 86400000
    };
} // namespace anonymous

namespace brfc {

Time::Time(int hour,
           int minute,
           int second,
           int msec)
        : msec_(0) {
    msec_ = hour * MSECS_IN_HOUR +
            minute * MSECS_IN_MIN +
            second * MSECS_IN_SEC +
            msec;
    if (hour < 0 or hour >= HOURS_IN_DAY)
        throw value_error("hour value out of range");
    if (minute < 0 or minute >= MINS_IN_HOUR)
        throw value_error("minute value out of range");
    if (second < 0 or second >= SECS_IN_MIN)
        throw value_error("second value out of range");
    if (msec < 0 or msec >= MSECS_IN_SEC)
        throw value_error("msec value out of range");
    if (msec_ >= MSECS_IN_DAY)
        throw value_error("time value out of range");
}

Time::Time(unsigned int msec)
        : msec_(msec) {
    if (msec_ >= MSECS_IN_DAY)
        throw value_error("time value out of range");
}

Time::Time(const Time& other)
        : msec_(other.msec_) {
}

Time&
Time::operator=(const Time& rhs) {
    if (this != &rhs) {
        msec_ = rhs.msec_;
    }
    return *this;
}

Time
Time::now() {
    struct timeval tv;
    if (gettimeofday(&tv, 0) == -1)
        throw std::runtime_error(strerror(errno));
    struct tm* tm;
    if ((tm = std::localtime(&tv.tv_sec)) == 0)
        throw std::runtime_error("std::localtime failed");
    return Time(tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
}

Time
Time::utc_now() {
    struct timeval tv;
    if (gettimeofday(&tv, 0) == -1)
        throw std::runtime_error(strerror(errno));
    struct tm* tm;
    if ((tm = std::gmtime(&tv.tv_sec)) == 0)
        throw std::runtime_error("std::gmtime failed");
    return Time(tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
}

int
Time::hour() const {
    return msec_ / MSECS_IN_HOUR;
}

int
Time::minute() const {
    return (msec_ % MSECS_IN_HOUR) / MSECS_IN_MIN;
}

int
Time::second() const {
    return (msec_ / MSECS_IN_SEC) % SECS_IN_MIN;
}

int
Time::msec() const {
    return msec_ % MSECS_IN_SEC;
}

void
Time::hour(int _hour) {
    if (_hour < 0 or _hour >= HOURS_IN_DAY)
        throw value_error("invalid hour");
    msec_ = msec_ - (hour() * MSECS_IN_HOUR) + (_hour * MSECS_IN_HOUR);
}

void
Time::minute(int _minute) {
    if (_minute < 0 or _minute >= MINS_IN_HOUR)
        throw value_error("invalid minute");
    msec_ = msec_ - (minute() * MSECS_IN_MIN) + (_minute * MSECS_IN_MIN);
}

void
Time::second(int _second) {
    if (_second < 0 or _second >= SECS_IN_MIN)
        throw value_error("invalid second");
    msec_ = msec_ - (second() * MSECS_IN_SEC) + (_second * MSECS_IN_SEC);
}

void
Time::msec(int _msec) {
    if (_msec < 0 or _msec >= MSECS_IN_SEC)
        throw value_error("invalid msec");
    msec_ = msec_ - msec() + _msec;
}

Time
Time::add_hours(int hours) const {
    return add_msecs(hours * MSECS_IN_HOUR);
}

Time
Time::add_minutes(int minutes) const {
    return add_msecs(minutes * MSECS_IN_MIN);
}

Time
Time::add_seconds(int seconds) const {
    return add_msecs(seconds * MSECS_IN_SEC);
}

Time
Time::add_msecs(int msecs) const {
    if (msecs < 0) {
        int negdays = (MSECS_IN_DAY - msecs) / MSECS_IN_DAY;
        msecs = msecs + negdays * MSECS_IN_DAY;
    }
    return Time((msec_ + msecs) % MSECS_IN_DAY);
}

Time
Time::from_string(const String& str, const String& format) {
    DateTimeParser parser(format);
    if (not parser.is_format_time_only())
        throw value_error("invalid format for time: "
                          + format.to_std_string());
    return parser.time_from_string(str);
}

Time
Time::from_iso_string(const String& str) {
    return from_string(str, "hhmmss");
}

Time
Time::from_extended_iso_string(const String& str) {
    return from_string(str, "hh:mm:ss");
}

String
Time::to_string(const String& format) const {
    DateTimeParser parser(format);
    if (not parser.is_format_time_only())
        throw value_error("invalid format for time: "
                          + format.to_std_string());
    return parser.time_to_string(*this);
}

} // namespace brfc
