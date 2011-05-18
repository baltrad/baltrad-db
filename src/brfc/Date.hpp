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

#ifndef BRFC_DATE_HPP
#define BRFC_DATE_HPP

#include <string>

namespace brfc {

class TimeDelta;

/**
 * @brief date in a proleptic Gregorian calendar
 * @ingroup exposed
 */
class Date {
  public:
    /**
     * @brief construct from year, month, day values
     * @throw value_error if the resulting date is not valid
     */
    Date(int year, int month, int day);
    
    /**
     * @brief copy constructor
     */
    Date(const Date& other)
            : jdn_(other.jdn_) {
    }
    
    /**
     * @brief copy assignment
     */
    Date& operator=(const Date& rhs) {
        if (this != &rhs) {
            jdn_ = rhs.jdn_;
        }
        return *this;
    }

    static Date today();

    static Date utc_today();

    static Date current_date() {
        return today();
    }

    int year() const;
    int month() const;
    int day() const;
    
    /**
     * @throw value_error if resulting date would be invalid
     */
    Date& year(int year);

    /**
     * @throw value_error if resulting date would be invalid
     */
    Date& month(int month);

    /**
     * @throw value_error if resulting date would be invalid
     */
    Date& day(int day);

    Date& operator+=(const TimeDelta& td);
    Date operator+(const TimeDelta& td) const;

    /**
     * @brief calculate a (proleptic) Gregorian date from Julian Day Number
     */
    static void date_from_jdn(unsigned int jdn,
                              int& year,
                              int& month,
                              int& day);
    
    /**
     * @brief calculate Julian Day Number from a (proleptic) Gregorian date
     * @throw value_error for dates before -4713-11-14
     */
    static unsigned int jdn_from_date(int year,
                                      int month,
                                      int day);
    
    /**
     * @brief number of days in a month of a year
     */
    static int days_in_month(int year, int month);
    
    /**
     * @brief check if a year is leap year
     */
    static bool is_leap_year(int year);
    
    bool operator==(const Date& rhs) const {
        return jdn_ == rhs.jdn_;
    }

    bool operator!=(const Date& rhs) const {
        return not (*this == rhs);
    }

    bool operator<(const Date& rhs) const {
        return jdn_ < rhs.jdn_;
    }
    
    /**
     * @brief parse date in ISO8601 format from @c str
     *
     * @throw value_error if a valid date can not be parsed
     */
    static Date from_iso_string(const std::string& str);

    std::string to_iso_string(bool extended=false) const;

  private:
    explicit Date(unsigned int jdn);

    unsigned int jdn_; ///< Julian Day Number
};

} // namespace brfc

#endif // BRFC_DATE_HPP
