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

#ifndef BRFC_EXPR_EXPRESSION_HPP
#define BRFC_EXPR_EXPRESSION_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Element.hpp>

class QString;

namespace brfc {
namespace expr {

/**
 * @brief ABC for expressions
 */
class Expression : public Element {
  public:
    ExpressionPtr shared_from_this() const {
        return static_pointer_cast<Expression>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }
    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief this != rhs
     */
    BinaryOperatorPtr ne(ExpressionPtr rhs) const;

    /**
     * @brief this == rhs
     */
    BinaryOperatorPtr eq(ExpressionPtr rhs) const;

    /**
     * @brief this > rhs
     */
    BinaryOperatorPtr gt(ExpressionPtr rhs) const;

    /**
     * @brief this < rhs
     */
    BinaryOperatorPtr lt(ExpressionPtr rhs) const;

    /**
     * @brief this <= rhs
     */
    BinaryOperatorPtr le(ExpressionPtr rhs) const;

    /**
     * @brief this >= rhs
     */
    BinaryOperatorPtr ge(ExpressionPtr rhs) const;
    
    /**
     * @brief low <= this => high
     */
    BinaryOperatorPtr between(ExpressionPtr low,
                              ExpressionPtr high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief this AND rhs
     */
    BinaryOperatorPtr and_(ExpressionPtr rhs) const;
    
    /**
     * @brief this OR rhs
     */
    BinaryOperatorPtr or_(ExpressionPtr rhs) const;
    //@}

    /**
     * @brief surround with parentheses
     */
    ParenthesesPtr parentheses() const;

    /**
     * @brief label this expression
     */
    LabelPtr label(const QString& label) const;

  protected:
    Expression()
            : Element() {
    }
};

}
}

#endif // BRFC_EXPR_EXPRESSION_HPP
