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

#ifndef BRFC_TIME_HPP
#define BRFC_TIME_HPP

namespace brfc {

class String;

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
    ///@}
    
    /**
     * @name addition
     * @{
     */
    /**
     * @brief get a new time with hours before/after held time
     *
     * @note time wraps at midnight
     */
    Time add_hours(int hours) const;

    /**
     * @brief get a new time with minutes before/after held time
     *
     * @note time wraps at midnight
     */
    Time add_minutes(int minutes) const;

    /**
     * @brief get a new time with seconds before/after held time
     *
     * @note time wraps at midnight
     */
    Time add_seconds(int seconds) const;

    /**
     * @brief get a new time with milliseconds before/after held time
     *
     * @note time wraps at midnight
     */
    Time add_msecs(int msecs) const;
    ///@}
    
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
    ///@}

    /**
     * @brief parse time from string
     *
     * @throw value_error if a valid time can not be parsed
     */
    static Time from_string(const String& str, const String& format);

    /**
     * @brief format time to string
     *
     */
    String to_string(const String& format) const;

  private:
    explicit Time(unsigned int msec);

    unsigned int msec_;
};

} // namespace brfc

#endif // BRFC_TIME_HPP
