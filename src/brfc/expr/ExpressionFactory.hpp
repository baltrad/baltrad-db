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

#ifndef BRFC_EXPR_EXPRESSION_FACTORY_HPP
#define BRFC_EXPR_EXPRESSION_FACTORY_HPP

#include <string>

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/AttributePrototypes.hpp>

namespace brfc {

class Date;
class Time;
class DateTime;

namespace expr {

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
    AttributePtr attribute(const std::string& name) const;

    /**
     * @brief construct expr::Literal containing a string
     * @{
     */
    LiteralPtr string(const std::string& value) const;
    LiteralPtr string(const char* value) const;

    /**
     * @}
     */

    /**
     * @brief construct expr::Literal containing a 64-bit integer
     */
    LiteralPtr int64_(long long value) const;

    /**
     * @brief construct expr::Literal containing a double precision float
     * @deprecated use double_()
     */
    LiteralPtr double_(double value) const;

    /**
     * @brief construct expr::Literal containing a date
     */
    LiteralPtr date(int year, int month, int day) const;

    /**
     * @brief construct expr::Literal containing a date
     */
    LiteralPtr date(const Date& date) const;

    /**
     * @brief construct expr::Literal containing a date
     */
    LiteralPtr date(const DateTime& datetime) const;

    /**
     * @brief construct expr::Literal containing a time
     */
    LiteralPtr time(int hour, int minute, int second=0) const;

    /**
     * @brief construct expr::Literal containing a time
     */
    LiteralPtr time(const Time& time) const;

    /**
     * @brief construct expr::Literal containing a time
     */
    LiteralPtr time(const DateTime& datetime) const;

    /**
     * @brief construct expr::Literal containing a datetime
     */
    LiteralPtr datetime(const DateTime& datetime) const;

    /**
     * @brief construct expr::Literal containing a bool
     */
    LiteralPtr bool_(bool value) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    BinaryOperatorPtr ne(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs == rhs
     */
    BinaryOperatorPtr eq(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs > rhs
     */
    BinaryOperatorPtr gt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs < rhs
     */
    BinaryOperatorPtr lt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs <= rhs
     */
    BinaryOperatorPtr le(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs >= rhs
     */
    BinaryOperatorPtr ge(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief low <= expr => high
     */
    BinaryOperatorPtr between(const Expression& expr,
                              const Expression& low,
                              const Expression& high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    BinaryOperatorPtr and_(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief lhs OR rhs
     */
    BinaryOperatorPtr or_(const Expression& lhs, const Expression& rhs) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief lhs + rhs
     */
    BinaryOperatorPtr add(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief lhs - rhs
     */
    BinaryOperatorPtr sub(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs * rhs
     */
    BinaryOperatorPtr mul(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs / rhs
     */
    BinaryOperatorPtr div(const Expression& lhs, const Expression& rhs) const;
    ///@}

    /**
     * @brief surround expr with parentheses
     */
    ParenthesesPtr parentheses(const Expression& expr) const;
    
    /**
     * @name aggregate functions
     * @{
     */
    /**
     * @brief MIN function
     */
    FunctionPtr min(const Expression& expr) const;
    
    /**
     * @brief MAX function
     */
    FunctionPtr max(const Expression& expr) const;

    /**
     * @brief SUM function
     */
    FunctionPtr sum(const Expression& expr) const;
    
    /**
     * @brief COUNT function
     */
    FunctionPtr count(const Expression& expr) const;
    //@}

    /**
     * @brief combine date and time attribute to datetime
     * @param date name of the date attribute
     * @param time name of the time attribute
     * @throw value_error if attributes are of incorrect type
     */
    ExpressionPtr combined_datetime(const std::string& date,
                                    const std::string& time) const;
  
  private:
    AttributePrototypes prototypes_;
};

}
}

#endif // BRFC_EXPR_EXPRESSION_FACTORY_HPP
