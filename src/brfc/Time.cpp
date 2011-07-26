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

#include <brfc/Time.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/TimeDelta.hpp>
#include <brfc/util/iso8601.hpp>

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
        throw std::invalid_argument("hour value out of range");
    if (minute < 0 or minute >= MINS_IN_HOUR)
        throw std::invalid_argument("minute value out of range");
    if (second < 0 or second >= SECS_IN_MIN)
        throw std::invalid_argument("second value out of range");
    if (msec < 0 or msec >= MSECS_IN_SEC)
        throw std::invalid_argument("msec value out of range");
    if (msec_ >= MSECS_IN_DAY)
        throw std::invalid_argument("time value out of range");
}

Time::Time(unsigned int msec)
        : msec_(msec) {
    if (msec_ >= MSECS_IN_DAY)
        throw std::invalid_argument("time value out of range");
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
    return DateTime::now().time();
}

Time
Time::utc_now() {
    return DateTime::utc_now().time();
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

Time&
Time::hour(int _hour) {
    if (_hour < 0 or _hour >= HOURS_IN_DAY)
        throw std::invalid_argument("invalid hour");
    msec_ = msec_ - (hour() * MSECS_IN_HOUR) + (_hour * MSECS_IN_HOUR);
    return *this;
}

Time&
Time::minute(int _minute) {
    if (_minute < 0 or _minute >= MINS_IN_HOUR)
        throw std::invalid_argument("invalid minute");
    msec_ = msec_ - (minute() * MSECS_IN_MIN) + (_minute * MSECS_IN_MIN);
    return *this;
}

Time&
Time::second(int _second) {
    if (_second < 0 or _second >= SECS_IN_MIN)
        throw std::invalid_argument("invalid second");
    msec_ = msec_ - (second() * MSECS_IN_SEC) + (_second * MSECS_IN_SEC);
    return *this;
}

Time&
Time::msec(int _msec) {
    if (_msec < 0 or _msec >= MSECS_IN_SEC)
        throw std::invalid_argument("invalid msec");
    msec_ = msec_ - msec() + _msec;
    return *this;
}

Time&
Time::operator+=(const TimeDelta& td) {
    // td.msecs() is always positive
    msec_ = (td.msecs() + msec_) % MSECS_IN_DAY;
    return *this;
}

Time
Time::operator+(const TimeDelta& td) const {
    return Time(*this) += td;
}

Time
Time::from_iso_string(const std::string& str) {
    iso8601::time t;
    if (not iso8601::parse_time(str, t))
        throw std::invalid_argument("invalid ISO8601 time: " + str);
    return Time(t.hour, t.minute, t.second, t.usec / 1000);
}

std::string
Time::to_iso_string(bool extended) const {
    iso8601::time t(hour(), minute(), second(), msec() * 1000);
    if (extended)
        return iso8601::to_string(t, iso8601::format::EXTENDED);
    else
        return iso8601::to_string(t, iso8601::format::BASIC);
}

} // namespace brfc
