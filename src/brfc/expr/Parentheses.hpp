#ifndef BRFC_EXPR_PARENTHESES_HPP
#define BRFC_EXPR_PARENTHESES_HPP

#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

/**
 * @brief surround expression in parentheses
 *
 * @ingroup exposed_in_binds
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

    virtual void do_accept(Visitor& visitor) {
        return visitor.visit(*this);
    }

  private:
    ExpressionPtr expression_;
};

}
}

#endif // BRFC_EXPR_PARENTHESES_HPP
