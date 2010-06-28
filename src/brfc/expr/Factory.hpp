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

#ifndef BRFC_EXPR_FACTORY_HPP
#define BRFC_EXPR_FACTORY_HPP

#include <brfc/String.hpp>

#include <brfc/deprecate.hpp>
#include <brfc/expr/fwd.hpp>

namespace brfc {

class Date;
class Time;
class DateTime;

namespace expr {

/**
 * @brief factory for constructing expression elements
 */
class Factory {
  public:
    /**
     * @brief construct expr::Attribute
     */
    AttributePtr attribute(const String& name) const;

    /**
     * @brief construct expr::Literal containing a string
     * @{
     */
    LiteralPtr string(const String& value) const;
    LiteralPtr string(const char* value) const;

    /**
     * @}
     */


    /**
     * @brief construct expr::Literal containing a 64-bit integer
     * @deprecated use int64_()
     */
    DEPRECATED(LiteralPtr integer(long long value) const);

    /**
     * @brief construct expr::Literal containing a 64-bit integer
     */
    LiteralPtr int64_(long long value) const;

    /**
     * @brief construct expr::Literal containing a double precision float
     * @deprecated use double_()
     */
    DEPRECATED(LiteralPtr real(double value) const);

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
     * @brief construct expr::Literal containing a bool
     * @deprecated use bool_()
     */
    DEPRECATED(LiteralPtr boolean(bool value) const);

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
    BinaryOperatorPtr ne(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs == rhs
     */
    BinaryOperatorPtr eq(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs > rhs
     */
    BinaryOperatorPtr gt(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs < rhs
     */
    BinaryOperatorPtr lt(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs <= rhs
     */
    BinaryOperatorPtr le(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs >= rhs
     */
    BinaryOperatorPtr ge(ExpressionPtr lhs, ExpressionPtr rhs) const;
    
    /**
     * @brief low <= expr => high
     */
    BinaryOperatorPtr between(ExpressionPtr expr,
                              ExpressionPtr low,
                              ExpressionPtr high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    BinaryOperatorPtr and_(ExpressionPtr lhs, ExpressionPtr rhs) const;
    
    /**
     * @brief lhs OR rhs
     */
    BinaryOperatorPtr or_(ExpressionPtr lhs, ExpressionPtr rhs) const;
    //@}

    /**
     * @brief surround expr with parentheses
     */
    ParenthesesPtr parentheses(ExpressionPtr xpr) const;
};

}
}

#endif // BRFC_EXPR_FACTORY_HPP
