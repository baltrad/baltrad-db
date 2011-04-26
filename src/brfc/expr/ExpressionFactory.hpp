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

#ifndef BRFC_EXPR_EXPRESSION_FACTORY_HPP
#define BRFC_EXPR_EXPRESSION_FACTORY_HPP

#include <string>

#include <brfc/expr/AttributePrototypes.hpp>

namespace brfc {

class Date;
class DateTime;
class Time;
class TimeDelta;
class Variant;

namespace expr {

class sexp;

/**
 * @brief factory for constructing expression elements
 */
class ExpressionFactory {
  public:
    /**
     * @brief default constructor
     *
     * use AttributePrototypes::default_odim_h5() for prototypes
     */
    ExpressionFactory();
    
    /**
     * @brief constructor
     */
    explicit ExpressionFactory(const AttributePrototypes& prototypes);

    /**
     * @brief construct expr::Attribute
     */
    sexp attribute(const std::string& name) const;

    sexp literal(const Variant& value) const;

    /**
     * @brief construct sexp containing a string
     */
    sexp string(const std::string& value) const;

    /**
     * @brief construct sexp containing a string
     */
    sexp string(const char* value) const;

    /**
     * @brief construct sexp containing a 64-bit integer
     */
    sexp int64_(long long value) const;
    
    /**
     * @brief construct sexp containing a 64-bit integer
     */
    sexp long_(long long value) const;

    /**
     * @brief construct sexp containing a double precision float
     */
    sexp double_(double value) const;

    /**
     * @brief construct sexp containing a date
     */
    sexp date(int year, int month, int day) const;

    /**
     * @brief construct sexp containing a date
     */
    sexp date(const Date& date) const;

    /**
     * @brief construct sexp containing a date
     */
    sexp date(const DateTime& datetime) const;

    /**
     * @brief construct sexp containing a time
     */
    sexp time(int hour, int minute, int second=0) const;

    /**
     * @brief construct sexp containing a time
     */
    sexp time(const Time& time) const;

    /**
     * @brief construct sexp containing a time
     */
    sexp time(const DateTime& datetime) const;

    /**
     * @brief construct sexp containing a datetime
     */
    sexp datetime(const DateTime& datetime) const;

    /**
     * @brief construct sexp containing a timedelta
     */
    sexp timedelta(const TimeDelta& delta) const;

    /**
     * @brief construct sexp containing a bool
     */
    sexp bool_(bool value) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    sexp ne(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs == rhs
     */
    sexp eq(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs > rhs
     */
    sexp gt(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs < rhs
     */
    sexp lt(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs <= rhs
     */
    sexp le(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs >= rhs
     */
    sexp ge(const sexp& lhs, const sexp& rhs) const;
    
    /**
     * @brief low <= expr <= high
     */
    sexp between(const sexp& x, const sexp& low, const sexp& high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    sexp and_(const sexp& lhs, const sexp& rhs) const;
    
    /**
     * @brief expr1 AND expr2 AND ...
     */
    sexp and_(const sexp& x) const;

    /**
     * @brief lhs OR rhs
     */
    sexp or_(const sexp& lhs, const sexp& rhs) const;
    
    /**
     * @brief expr1 OR expr2 OR ...
     */
    sexp or_(const sexp& x) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief lhs + rhs
     */
    sexp add(const sexp& lhs, const sexp& rhs) const;
    
    /**
     * @brief lhs - rhs
     */
    sexp sub(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs * rhs
     */
    sexp mul(const sexp& lhs, const sexp& rhs) const;

    /**
     * @brief lhs / rhs
     */
    sexp div(const sexp& lhs, const sexp& rhs) const;
    ///@}

    /**
     * @brief surround x with parentheses
     */
    sexp parentheses(const sexp& x) const;
    
    /**
     * @brief x LIKE pattern
     */
    sexp like(const sexp& x, const std::string& pattern) const;
    
    /**
     * @brief x IN l
     */
    sexp in(const sexp& x, const sexp& l) const;
    
    /**
     * @brief x NOT IN l
     */
    sexp not_in(const sexp& x, const sexp& l) const;
    
    /**
     * @name aggregate functions
     * @{
     */
    /**
     * @brief MIN function
     */
    sexp min(const sexp& expr) const;
    
    /**
     * @brief MAX function
     */
    sexp max(const sexp& expr) const;

    /**
     * @brief SUM function
     */
    sexp sum(const sexp& x) const;
    
    /**
     * @brief COUNT function
     */
    sexp count(const sexp& x) const;
    //@}

    /**
     * @brief combine date and time attribute to datetime
     * @param date name of the date attribute
     * @param time name of the time attribute
     * @throw value_error if attributes are of incorrect type
     */
    sexp combined_datetime(const std::string& date,
                           const std::string& time) const;
  
  private:
    sexp binary(const std::string& op, const sexp& lhs, const sexp& rhs) const;
    sexp unary(const std::string& op, const sexp& arg) const;

    AttributePrototypes prototypes_;
};

}
}

#endif // BRFC_EXPR_EXPRESSION_FACTORY_HPP
