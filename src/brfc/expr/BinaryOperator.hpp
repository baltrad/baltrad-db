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

#ifndef BRFC_EXPR_BINARY_OPERATOR_HPP
#define BRFC_EXPR_BINARY_OPERATOR_HPP

#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

/**
 * @brief operator operating on two expressions
 */
class BinaryOperator : public Expression {
  public:
    static BinaryOperatorPtr create(const QString& op,
                                    ExpressionPtr lhs,
                                    ExpressionPtr rhs) {
        return BinaryOperatorPtr(new BinaryOperator(op, lhs, rhs));
    }

    void lhs(ExpressionPtr lhs) { lhs_ = lhs; }
    void rhs(ExpressionPtr rhs) { rhs_ = rhs; }

    ExpressionPtr lhs() const { return lhs_; }
    ExpressionPtr rhs() const { return rhs_; }
    const QString& op() const { return op_; }

  protected:
    BinaryOperator(const QString& op,
                   ExpressionPtr lhs,
                   ExpressionPtr rhs)
            : Expression()
            , op_(op)
            , lhs_(lhs)
            , rhs_(rhs) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    QString op_;
    ExpressionPtr lhs_;
    ExpressionPtr rhs_;
};

}
}

#endif // BRFC_EXPR_BINARY_OPERATOR_HPP
