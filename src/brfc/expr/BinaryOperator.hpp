#ifndef BRFC_EXPR_BINARY_OPERATOR_HPP
#define BRFC_EXPR_BINARY_OPERATOR_HPP

#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

/**
 * @brief operator operating on two expressions
 * @ingroup exposed_in_binds
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
