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
#include <boost/numeric/conversion/cast.hpp>

#include <brfc/deprecate.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/Date.hpp>
#include <brfc/String.hpp>
#include <brfc/Time.hpp>

namespace brfc {

class Variant {
  public:
    enum Type {
        NONE,
        STRING,
        INT64,
        LONGLONG = INT64, // XXX: deprecated
        DOUBLE,
        BOOL,
        DATE,
        TIME
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
     * @brief construct longlong variant
     */
    explicit Variant(unsigned long long value)
            : type_(INT64)
            , value_(boost::numeric_cast<long long>(value)) {
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
     * @brief copy assignment
     */
    Variant& operator=(const Variant& rhs) {
        if (this != &rhs) {
            type_ = rhs.type_;
            value_ = rhs.value_;
        }
        return *this;
    }

    Type type() const { return type_; }

    bool is_null() const;
    bool is_string() const;

    /**
     * @deprecated use is_int64()
     */
    DEPRECATED(bool is_long() const);

    bool is_int64() const;

    bool is_double() const;
    bool is_bool() const;
    bool is_date() const;
    bool is_time() const;

    const String& string() const;
    
    /**
     * @deprecated use int64_()
     */
    DEPRECATED(long long longlong() const);

    long long int64_() const;

    double double_() const;

    bool bool_() const;

    const Date& date() const;

    const Time& time() const;

    /**
     * @brief convert to String
     */
    String to_string() const;

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
                           Time> ValueType;
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
