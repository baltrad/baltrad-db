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

#ifndef BRFC_EXPR_BINARY_OPERATOR_HPP
#define BRFC_EXPR_BINARY_OPERATOR_HPP

#include <string>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

/**
 * @brief operator operating on two expressions
 */
class BinaryOperator : public Expression {
  public:
    enum Op {
        NE,
        EQ,
        GT,
        LT,
        LE,
        GE,
        LIKE,
        IN,
        NOT_IN,
        AND,
        OR,
        ADD,
        SUB,
        MUL,
        DIV
    };

    BinaryOperator(Op op,
                   const Expression& lhs,
                   const Expression& rhs)
            : Expression()
            , op_(op)
            , lhs_(lhs.clone())
            , rhs_(rhs.clone()) {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new BinaryOperator(*this));
    }

    void lhs(ExpressionPtr lhs) { lhs_ = lhs; }
    void rhs(ExpressionPtr rhs) { rhs_ = rhs; }

    ExpressionPtr lhs() const { return lhs_; }
    ExpressionPtr rhs() const { return rhs_; }
    Op op() const { return op_; }

  protected:
    BinaryOperator(const BinaryOperator& other)
            : op_(other.op_)
            , lhs_(other.lhs_->clone())
            , rhs_(other.rhs_->clone()) {
    }

    bool do_equals(const Expression& other) const {
        const BinaryOperator* optr = dynamic_cast<const BinaryOperator*>(&other);
        if (optr and op_ == optr->op_) {
            if (lhs_->equals(*optr->lhs_) and rhs_->equals(*optr->rhs_)) {
                return true;
            } else if (lhs_->equals(*optr->rhs_) and rhs_->equals(*optr->lhs_)) {
                return true;
            }
        }
        return false;
    }

  private:
    Op op_;
    ExpressionPtr lhs_;
    ExpressionPtr rhs_;
};

}
}

#endif // BRFC_EXPR_BINARY_OPERATOR_HPP
