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

#include <string>

namespace brfc {

/**
 * @brief iso8601 date and time handling
 * 
 * this doesn't attempt to be a full (and strict) implementation of ISO8601.
 * It's just to cover our needs.
 */
namespace iso8601 {

struct date {
    explicit date(int year_=0,
                  unsigned int month_=0,
                  unsigned int day_=0)
        : year(year_)
        , month(month_)
        , day(day_) {
    }

    int year;
    unsigned int month;
    unsigned int day;
};

struct time {
    explicit time(unsigned int hour_=0,
                  unsigned int minute_=0,
                  unsigned int second_=0,
                  unsigned int usec_=0)
        : hour(hour_)
        , minute(minute_)
        , second(second_)
        , usec(usec_) {
    }

    unsigned int hour;
    unsigned int minute;
    unsigned int second;
    unsigned int usec;
};

struct datetime {
    explicit datetime(int year_=0,
                      unsigned int month_=0,
                      unsigned int day_=0,
                      unsigned int hour_=0,
                      unsigned int minute_=0,
                      unsigned int second_=0,
                      unsigned int usec_=0)
            : date(year_, month_, day_)
            , time(hour_, minute_, second_, usec_) {
    }

    struct date date;
    struct time time;
};

struct format {
    enum type {
        BASIC,
        EXTENDED
    };
};

bool
parse_date(const std::string& str, date& d);

bool
parse_time(const std::string& str, time& t);

bool
parse_datetime(const std::string& str, datetime& dt);

std::string
to_string(const date& d, format::type fmt=format::BASIC);

std::string
to_string(const time& t, format::type fmt=format::BASIC);

std::string
to_string(const datetime& dt, format::type fmt=format::BASIC);

} // namespace iso8601
} // namepsace brfc
