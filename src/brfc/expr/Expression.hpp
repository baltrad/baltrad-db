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

#ifndef BRFC_EXPR_EXPRESSION_HPP
#define BRFC_EXPR_EXPRESSION_HPP

#include <boost/noncopyable.hpp>

#include <brfc/expr/fwd.hpp>

namespace brfc {
namespace expr {

class sexp;

/**
 * @brief ABC for query expressions
 */
class Expression : public boost::noncopyable {
  public:
    /**
     * @brief constructor
     */
    Expression()
            : boost::noncopyable() {
    }
    
    /**
     * @brief create a copy of this expression
     */
    virtual ExpressionPtr clone() const = 0;

    virtual sexp to_sexp() const = 0;
    
    /**
     * @brief destructor
     */
    virtual ~Expression() { }

    /**
     * @brief test equality of two expressions
     * @return true if this is equal to @c other
     */
    bool equals(const Expression& other) const {
        return do_equals(other);
    }
    
    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief this != rhs
     */
    BinaryOperatorPtr ne(const Expression& rhs) const;

    /**
     * @brief this == rhs
     */
    BinaryOperatorPtr eq(const Expression& rhs) const;

    /**
     * @brief this > rhs
     */
    BinaryOperatorPtr gt(const Expression& rhs) const;

    /**
     * @brief this < rhs
     */
    BinaryOperatorPtr lt(const Expression& rhs) const;

    /**
     * @brief this <= rhs
     */
    BinaryOperatorPtr le(const Expression& rhs) const;

    /**
     * @brief this >= rhs
     */
    BinaryOperatorPtr ge(const Expression& rhs) const;
    
    /**
     * @brief low <= this => high
     */
    BinaryOperatorPtr between(const Expression& low,
                              const Expression& high) const;
    
    /**
     * @brief this LIKE pattern
     * @param pattern pattern to match against
     *
     * perform simple pattern matching on this expression.
     * In the pattern, '*' matches any number of characters, '?' matches
     * single character.
     */
    BinaryOperatorPtr like(const std::string& pattern) const;
    
    /**
     *  @brief this LIKE pattern
     *  @param pattern pattern to match against
     *  @sa like(const std::string&) const
     **/
    BinaryOperatorPtr like(const Expression& pattern) const;
    
    /**
     * @brief this IN values
     */
    BinaryOperatorPtr in(const ExpressionList& values) const;
    
    /**
     * @brief this NOT IN values
     */
    BinaryOperatorPtr not_in(const ExpressionList& values) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief this AND rhs
     */
    BinaryOperatorPtr and_(const Expression& rhs) const;
    
    /**
     * @brief this OR rhs
     */
    BinaryOperatorPtr or_(const Expression& rhs) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief this + rhs
     */
    BinaryOperatorPtr add(const Expression& rhs) const;

    /**
     * @brief this - rhs
     */
    BinaryOperatorPtr sub(const Expression& rhs) const;

    /**
     * @brief this * rhs
     */
    BinaryOperatorPtr mul(const Expression& rhs) const;

    /**
     * @brief this * rhs
     */
    BinaryOperatorPtr div(const Expression& rhs) const;
    //@}

    /**
     * @brief surround with parentheses
     */
    ParenthesesPtr parentheses() const;

  protected:
    /**
     * @brief copy-constructor
     */
    Expression(const Expression& other);

    virtual bool do_equals(const Expression& other) const = 0;
};

}
}

#endif // BRFC_EXPR_EXPRESSION_HPP
