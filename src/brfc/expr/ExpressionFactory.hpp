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
class Expression;
class Time;
class TimeDelta;
class Variant;

/**
 * @brief factory for constructing expression elements
 * @ingroup exposed
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
     * @brief construct Attribute
     */
    Expression attribute(const std::string& name) const;

    Expression attribute(const std::string& name, const std::string& type) const;

    Expression literal(const Variant& value) const;

    /**
     * @brief construct Expression containing a string
     */
    Expression string(const std::string& value) const;

    /**
     * @brief construct Expression containing a string
     */
    Expression string(const char* value) const;

    /**
     * @brief construct Expression containing a 64-bit integer
     */
    Expression int64_(long long value) const;
    
    /**
     * @brief construct Expression containing a 64-bit integer
     */
    Expression long_(long long value) const;

    /**
     * @brief construct Expression containing a double precision float
     */
    Expression double_(double value) const;

    /**
     * @brief construct Expression containing a date
     */
    Expression date(int year, int month, int day) const;

    /**
     * @brief construct Expression containing a date
     */
    Expression date(const Date& date) const;

    /**
     * @brief construct Expression containing a date
     */
    Expression date(const DateTime& datetime) const;

    /**
     * @brief construct Expression containing a time
     */
    Expression time(int hour, int minute, int second=0) const;

    /**
     * @brief construct Expression containing a time
     */
    Expression time(const Time& time) const;

    /**
     * @brief construct Expression containing a time
     */
    Expression time(const DateTime& datetime) const;

    /**
     * @brief construct Expression containing a datetime
     */
    Expression datetime(const DateTime& datetime) const;

    /**
     * @brief construct Expression containing a timedelta
     */
    Expression timedelta(const TimeDelta& delta) const;

    /**
     * @brief construct Expression containing a bool
     */
    Expression bool_(bool value) const;


    Expression binary_op(const std::string& op, const Expression& lhs, const Expression& rhs) const;
    Expression unary_op(const std::string& op, const Expression& arg) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    Expression ne(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs == rhs
     */
    Expression eq(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs > rhs
     */
    Expression gt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs < rhs
     */
    Expression lt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs <= rhs
     */
    Expression le(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs >= rhs
     */
    Expression ge(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief low <= expr <= high
     */
    Expression between(const Expression& x, const Expression& low, const Expression& high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    Expression and_(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief expr1 AND expr2 AND ...
     */
    Expression and_(const Expression& x) const;

    /**
     * @brief lhs OR rhs
     */
    Expression or_(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief expr1 OR expr2 OR ...
     */
    Expression or_(const Expression& x) const;

    /**
     * @brief NOT x
     */
    Expression not_(const Expression& x) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief lhs + rhs
     */
    Expression add(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief lhs - rhs
     */
    Expression sub(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs * rhs
     */
    Expression mul(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs / rhs
     */
    Expression div(const Expression& lhs, const Expression& rhs) const;
    ///@}

    /**
     * @brief surround x with parentheses
     */
    Expression parentheses(const Expression& x) const;
    
    /**
     * @brief x LIKE pattern
     */
    Expression like(const Expression& x, const std::string& pattern) const;
    
    /**
     * @brief x IN l
     */
    Expression in(const Expression& x, const Expression& l) const;
    
    /**
     * @name aggregate functions
     * @{
     */
    /**
     * @brief MIN function
     */
    Expression min(const Expression& expr) const;
    
    /**
     * @brief MAX function
     */
    Expression max(const Expression& expr) const;

    /**
     * @brief SUM function
     */
    Expression sum(const Expression& x) const;
    
    /**
     * @brief COUNT function
     */
    Expression count(const Expression& x) const;
    //@}

    /**
     * @brief combine date and time attribute to datetime
     * @param date name of the date attribute
     * @param time name of the time attribute
     * @throw value_error if attributes are of incorrect type
     */
    Expression combined_datetime(const std::string& date,
                           const std::string& time) const;
  
  private:

    AttributePrototypes prototypes_;
};

} // namespace brfc

#endif // BRFC_EXPR_EXPRESSION_FACTORY_HPP
