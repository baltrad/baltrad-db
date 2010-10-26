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

#ifndef BRFC_VARIANT_HPP
#define BRFC_VARIANT_HPP

#include <boost/variant.hpp>

#include <brfc/deprecate.hpp>
#include <brfc/String.hpp>
#include <brfc/DateTime.hpp>

namespace brfc {

/**
 * @brief Variant value
 */
class Variant {
  public:
    enum Type {
        NONE,
        STRING,
        INT64,
        DOUBLE,
        BOOL,
        DATE,
        TIME,
        DATETIME
    };

    /**
     * @brief construct empty variant
     */
    Variant()
            : type_(NONE)
            , value_() {
    }

    /**
     * @brief copy constructor
     */
    Variant(const Variant& other)
            : type_(other.type_)
            , value_(other.value_) {
    }

    /**
     * @brief construct string variant from char array
     */
    explicit Variant(const char* value)
            : type_(STRING)
            , value_(String::from_utf8(value)) {
    }

    /**
     * @brief construct string variant from String
     */
    explicit Variant(const String& value)
            : type_(STRING)
            , value_(value) {
    }

    /**
     * @brief construct longlong variant
     */
    explicit Variant(int value)
            : type_(INT64)
            , value_(static_cast<long long>(value)) {
    
    }

    /**
     * @brief construct longlong variant
     */
    explicit Variant(unsigned int value)
            : type_(INT64)
            , value_(static_cast<long long>(value)) {
    
    }

    /**
     * @brief construct longlong variant
     */
    explicit Variant(long long value)
            : type_(INT64)
            , value_(value) {
    }

    /**
     * @brief construct double variant
     */
    explicit Variant(double value)
            : type_(DOUBLE)
            , value_(value) {
    }

    /**
     * @brief construct bool variant
     */
    explicit Variant(bool value)
            : type_(BOOL)
            , value_(value) {
    }

    /**
     * @brief construct date variant
     */
    explicit Variant(const Date& value)
            : type_(DATE)
            , value_(value) {
    }

    /**
     * @brief construct time variant
     */
    explicit Variant(const Time& value)
            : type_(TIME)
            , value_(value) {
    }
    
    /**
     * @brief construct datetime variant
     */
    explicit Variant(const DateTime& value)
            : type_(DATETIME)
            , value_(value) {

    }

    /**
     * @brief copy assignment
     */
    Variant& operator=(const Variant& rhs) {
        if (this != &rhs) {
            type_ = rhs.type_;
            value_ = rhs.value_;
        }
        return *this;
    }
    
    /**
     * @name type determination
     * @{
     */
    /**
     * @brief enumerated type
     */
    Type type() const { return type_; }
    
    /**
     * @brief test for NULL variant
     */
    bool is_null() const;

    /**
     * @brief test for STRING variant
     */
    bool is_string() const;

    /**
     * @brief test for INT64 variant
     */
    bool is_int64() const;
    
    /**
     * @brief test for DOUBLE variant
     */
    bool is_double() const;

    /**
     * @brief test for BOOL variant
     */
    bool is_bool() const;

    /**
     * @brief test for DATE variant
     */
    bool is_date() const;

    /**
     * @brief test for DATETIME variant
     */
    
    /**
     * @brief test for TIME variant
     */
    bool is_time() const;
    ///@}
    
    /**
     * @name strict value access
     * @{
     */
    /**
     * @brief access STRING variant
     * @throw value_error if type() != STRING
     */
    const String& string() const;
    
    /**
     * @brief access INT64 variant
     * @throw value_error if type() != INT64
     */
    long long int64_() const;

    /**
     * @brief access DOUBLE variant
     * @throw value_error if type() != DOUBLE
     */
    double double_() const;

    /**
     * @brief access BOOL variant
     * @throw value_error if type() != BOOL
     */
    bool bool_() const;

    /**
     * @brief access DATE variant
     * @throw value_error if type() != DATE
     */
    const Date& date() const;

    /**
     * @brief access TIME variant
     * @throw value_error if type() != TIME
     */
    const Time& time() const;

    /**
     * @brief access DATETIME variant
     * @throw value_error if type() != DATETIME
     */
    const DateTime& datetime() const;
    ///@}
    
    /**
     * @name value conversion
     * @{
     */
    /**
     * @brief convert to String
     *
     * all variants are convertible to string.
     *
     * conversions:
     *  - NULL variant is converted to empty String
     *  - double and int64 variants are converted using String::number
     *  - bool variant maps is converted to "True" or "False"
     *  - Date variant is converted to ISO 8601 format yyyy-MM-dd
     *  - Time variant is converted to ISO 8601 format hh:mm:ss
     *  - DateTime variant is converted to ISO 8601 format yyyy-MM-dd hh:mm:ss
     */
    String to_string() const;
    
    /**
     * @brief convert to 64-bit integer
     * @throw value_error if not convertible
     * 
     * conversions:
     *  - NULL variant is converted to 0
     *  - String values are converted with boost::lexical_cast
     *  - double and bool variants are converted with implicit C++ conversion
     *  - all other variants are not convertible
     */
    long long to_int64() const;
    
    /**
     * @brief convert to double precision floating-point number
     *
     * conversions:
     *  - NULL variant is converted to 0.0
     *  - String values are converted with boost::lexical_cast
     *  - int64 and bool variants are converted with implicit C++ conversion
     *  - all other variants are not convertible
     */
    double to_double() const;
    
    /**
     * @brief convert to bool value
     * 
     * all variants are convertible to bool
     *
     * conversions:
     *  - NULL variant is converted to false
     *  - empty String and String containing "0" or "false" is converted to
     *    false, all other strings to true
     *  - Date and Time variants are converted to true
     *  - all other variants are converted using implicit C++ conversion
     */
    bool to_bool() const;
    
    /**
     * @brief convert to Time value
     *
     * conversions:
     *  - String variant containing ISO 8601 format 'yyyy-MM-dd'
     *  - DateTime values convert to contained Time value
     *  - all other variants are not convertible
     */
    Time to_time() const;
    
    /**
     * @brief convert to Date value
     *
     * conversions:
     *  - String variant containing ISO 8601 format 'hh:mm:ss'
     *  - DateTime values convert to contained Date value
     *  - all other variants are not convertible
     */
    Date to_date() const;
    ///@}

    /**
     * @brief convert to DateTime value
     *
     * conversions:
     *  - String variant containing ISO 8601 format 'yyyy-MM-dd hh:mm:ss'
     *  - all other variants are not convertible
     */
    DateTime to_datetime() const;

  private:
    friend bool operator==(const Variant&, const Variant&);

    template<typename T>
    T get() const;

    Type type_;
    typedef boost::variant<String,
                           long long,
                           double,
                           bool,
                           Date,
                           Time,
                           DateTime> ValueType;
    ValueType value_;
};

/**
 * @brief equality comparison
 */
bool operator==(const Variant& lhs, const Variant& rhs);

/**
 * @brief inequality comparison
 */
inline
bool operator!=(const Variant& lhs, const Variant& rhs) {
    return not (lhs == rhs);
}

} // namespace brfc

#endif // BRFC_VARIANT_HPP
