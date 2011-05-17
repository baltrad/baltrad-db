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

#ifndef BRFC_OH5_VALUE_HPP
#define BRFC_OH5_VALUE_HPP

#include <boost/variant.hpp>

#include <string>

namespace brfc {

class Date;
class Time;

/**
 * ODIM_H5 scalar value
 */
class Oh5Scalar {
  public:
    enum Type {
        STRING,
        INT64,
        LONG = INT64,
        DOUBLE
    };
  
    /**
     * @brief construct string value from char array
     */
    explicit Oh5Scalar(const char* value)
            : value_(std::string(value)) {
    }
    
    /**
     * @brief construct string value
     */
    explicit Oh5Scalar(const std::string& value)
            : value_(value) {
    }
    
    /**
     * @brief construct int64 value
     * @note to resolve ambiguity
     **/
    explicit Oh5Scalar(int value)
            : value_(static_cast<long long>(value)) {
    
    }

    /**
     * @brief construct int64 value
     */
    explicit Oh5Scalar(long long value)
            : value_(value) {
    }

    /**
     * @brief construct double value
     */
    explicit Oh5Scalar(double value)
            : value_(value) {
    }

    /**
     * @brief construct string value from bool
     */
    explicit Oh5Scalar(bool value);

    /**
     * @brief construct string value from Date
     */
    explicit Oh5Scalar(const Date& value);

    /**
     * @brief construct string value from Time
     */
    explicit Oh5Scalar(Time value);
   
    /**
     * @brief copy constructor
     */
    Oh5Scalar(const Oh5Scalar& other)
            : value_(other.value_) {
    }

    /**
     * @brief copy assignment
     */

    Oh5Scalar& operator=(const Oh5Scalar& rhs) {
        if (this != &rhs) {
            value_ = rhs.value_;
        }
        return *this;
    }

    Type type() const;

    /**
     * @name strict value access
     * @{
     */
    /**
     * @brief access string value
     */
    const std::string& string() const;
    
    /**
     * @brief access int64 value
     */
    long long int64_() const;
    
    /**
     * @brief access int64 value
     */
    long long long_() const { return int64_(); }
    
    /**
     * @brief access double value
     */
    double double_() const;
    ///@}
    
    /**
     * @name value conversion
     */
    /**
     * @brief convert to std::string
     */
    std::string to_string() const;
    
    /**
     * @brief convert to int64
     */
    long long to_int64() const;

    /**
     * @brief convert to int64
     */
    long long to_long() const { return to_int64(); }

    /**
     * @brief convert to double
     */
    double to_double() const;
    
    /**
     * @brief convert to Date
     * @throw value_error if conversion fails
     *
     * convert from string to Date using 'yyyyMMdd' as format
     */
    Date to_date() const;

    /**
     * @brief convert to Time
     * @throw value_error if conversion fails
     *
     * convert from string to Time using 'hhmmss' as format
     */
    Time to_time() const;
    
    /**
     * @brief convert to bool
     * @throw value_error if conversion fails
     */
    bool to_bool() const;
  
  private:
    friend bool operator==(const Oh5Scalar&, const Oh5Scalar&);

    template<typename T>
    T get() const;

    typedef boost::variant<std::string, long long, double> ScalarType;

    ScalarType value_;
};

/**
 * @brief equality comparison
 */
bool operator==(const Oh5Scalar& lhs, const Oh5Scalar& rhs);

/**
 * @brief inequality comaprison
 */
inline
bool operator!=(const Oh5Scalar& lhs, const Oh5Scalar& rhs) {
    return not (lhs == rhs);
}

} // namespace brfc

#endif // BRFC_OH5_VALUE_HPP