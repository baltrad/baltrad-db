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

#ifndef BRFC_EXPR_PARENTHESES_HPP
#define BRFC_EXPR_PARENTHESES_HPP

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

/**
 * @brief surround expression in parentheses
 */
class Parentheses : public Expression {
  public:
    explicit Parentheses(const Expression& expr)
            : Expression()
            , expr_(expr.clone()) {
    }
   
    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new Parentheses(*this));
    }

    void expression(const Expression& expr) {
        expr_ = expr.clone();
    }
    
    const Expression& expression() const {
        return *expr_;
    }

  protected:
    Parentheses(const Parentheses& other)
            : expr_(other.expr_->clone()) {
    }

    virtual bool do_equals(const Expression& other) const {
        const Parentheses* optr = dynamic_cast<const Parentheses*>(&other);
        if (optr and expr_->equals(*optr->expr_))
            return true;
        return false;
    }

  private:
    ExpressionPtr expr_;
};

}
}

#endif // BRFC_EXPR_PARENTHESES_HPP
