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

#ifndef BRFC_EXPR_LABEL_HPP
#define BRFC_EXPR_LABEL_HPP

#include <brfc/String.hpp>

#include <brfc/expr/Expression.hpp>


namespace brfc {
namespace expr {

/**
 * @brief labeled expression
 *
 * table.column AS label
 */
class Label : public Expression {
  public:
    Label(const Expression& expr, const String& name)
            : expr_(expr.clone())
            , name_(name) {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new Label(*this));
    }

    void name(const String& name) {
        name_ = name;
    }

    const String& name() const {
        return name_;
    }

    void expression(const Expression& expr) {
        expr_ = expr.clone();
    }

    /**
     * @brief get expression this label is for
     */
    const Expression& expression() const {
        return *expr_;
    }

  protected:
    Label(const Label& other)
            : expr_(other.expr_->clone())
            , name_(other.name_) {
    }

  private:
    ExpressionPtr expr_;
    String name_;
};

}
}

#endif // BRFC_EXPR_LABEL_HPP
