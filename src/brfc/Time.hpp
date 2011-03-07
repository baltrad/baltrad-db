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

#ifndef BRFC_TIME_HPP
#define BRFC_TIME_HPP

#include <string>

namespace brfc {

class TimeDelta;

/**
 * @brief naive time implementation
 */
class Time {
  public:
    /**
     * @brief construct from specified hour/minute/second/millisecond values
     */
    Time(int hour,
         int minute,
         int second=0,
         int msec=0);
    
    /**
     * @brief copy constructor
     */
    Time(const Time& other);
    
    /**
     * @brief copy assignment
     */
    Time& operator=(const Time& rhs);
    
    /**
     * @brief construct containing current local time
     */
    static Time now();
    
    /**
     * @brief construct containing current UTC time
     */
    static Time utc_now();
    
    /**
     * @name accessors
     * @{
     */
    /**
     * @brief hour (0 - 23)
     */
    int hour() const;
    
    /**
     * @brief minute (0 - 59)
     */
    int minute() const;

    /**
     * @brief second (0 - 59)
     */
    int second() const;

    /**
     * @brief millisecond (0 - 999)
     */
    int msec() const;
    
    /**
     * @brief milliseconds since midnight
     */
    int cumulative_msecs() const { return msec_; }
    ///@}
    
    /**
     * @name mutators
     * @{
     */
    Time& hour(int hour);
    Time& minute(int minute);
    Time& second(int second);
    Time& msec(int msec);
    ///@}
    
    /**
     * @note time wraps at midnight
     */
    Time& operator+=(const TimeDelta& td);
    
    /**
     * @note time wraps at midnight
     */
    Time operator+(const TimeDelta& td) const;

    /**
     * @name comparison
     * @{
     */
    bool operator==(const Time& rhs) const {
        return msec_ == rhs.msec_;
    }

    bool operator!=(const Time& rhs) const {
        return not (*this == rhs);
    }

    bool operator<(const Time& rhs) const {
        return msec_ < rhs.msec_;
    }
    ///@}

    /**
     * @brief parse time from string
     *
     * @throw value_error if a valid time can not be parsed from the format
     */
    static Time from_string(const std::string& str, const std::string& format);
    
    /**
     * @brief parse time in format 'hhmmss' from @c str
     *
     * @throw value_error if a valid time can not be parsed
     */
    static Time from_iso_string(const std::string& str);
    
    /**
     * @brief parse time in format 'hh:mm:ss' from @c str
     *
     * @throw value_error if a valid time can not be parsed
     */
    static Time from_extended_iso_string(const std::string& str);

    /**
     * @brief format time to string
     *
     */
    std::string to_string(const std::string& format) const;

  private:
    explicit Time(unsigned int msec);

    unsigned int msec_;
};

} // namespace brfc

#endif // BRFC_TIME_HPP
