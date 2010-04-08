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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
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
    static ParenthesesPtr create(ExpressionPtr expression) {
        return ParenthesesPtr(new Parentheses(expression));
    }

    void expression(ExpressionPtr expression) {
        expression_ = expression;
    }
    
    ExpressionPtr expression() const {
        return expression_;
    }

  protected:
    Parentheses(ExpressionPtr expression)
            : Expression()
            , expression_(expression) {
    }

  private:
    ExpressionPtr expression_;
};

}
}

#endif // BRFC_EXPR_PARENTHESES_HPP
