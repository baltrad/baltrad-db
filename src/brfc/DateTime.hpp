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

#ifndef BRFC_DATE_TIME_HPP
#define BRFC_DATE_TIME_HPP

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

struct tm;

namespace brfc {

/**
 * @brief combined Date and Time
 */
class DateTime {
  public:
    explicit DateTime(int year=1, int month=1, int day=1,
                      int hour=0, int minute=0, int second=0, int msec=0)
            : date_(year, month, day)
            , time_(hour, minute, second, msec) {
    }
    
    /**
     * @brief construct from Date and Time
     */
    DateTime(const Date& date, const Time& time)
            : date_(date)
            , time_(time) {
    }

    explicit DateTime(const Date& date)
            : date_(date)
            , time_(0, 0, 0, 0) {
    }

    explicit DateTime(const Time& time)
            : date_(1, 1, 1)
            , time_(time) {
    }

    /**
     * @brief copy constructor
     */
    DateTime(const DateTime& other)
            : date_(other.date_)
            , time_(other.time_) {
    }

    /**
     * @brief copy assignment
     */
    DateTime& operator=(const DateTime& rhs) {
        if (this != &rhs) {
            date_ = rhs.date_;
            time_ = rhs.time_;
        }
        return *this;
    }
    
    /**
     * @brief construct containing current local time
     */
    static DateTime now();

    /**
     * @brief construct containing current UTC time
     */
    static DateTime utc_now();
    
    const Date& date() const { return date_; }
    const Time& time() const { return time_; }

    Date& date() { return date_; }
    Time& time() { return time_; }

    DateTime& date(const Date& date) { date_ = date; return *this; }
    DateTime& time(const Time& time) { time_ = time; return *this; }

    String to_string(const String& format) const;

    static DateTime from_string(const String& str, const String& format);
    
    /**
     * @brief convert to tm
     * @throw value_error if not convertible
     */
    struct tm to_tm() const;

    bool operator==(const DateTime& rhs) const;

    bool operator!=(const DateTime& rhs) const {
        return not (*this == rhs);
    }

    DateTime& operator+=(const TimeDelta& td);
    
    DateTime operator+(const TimeDelta& td) const;

  private:
    Date date_;
    Time time_;
};

} // namespace brfc

#endif // BRFC_DATE_TIME_HPP
