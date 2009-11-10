#ifndef BRFC_EXPR_EXPRESSION_HPP
#define BRFC_EXPR_EXPRESSION_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Element.hpp>
#include <string>

namespace brfc {
namespace expr {

/**
 * @brief ABC for expressions
 * @ingroup exposed_in_binds
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
    LabelPtr label(const std::string& label) const;

  protected:
    Expression()
            : Element() {
    }
};

}
}

#endif // BRFC_EXPR_EXPRESSION_HPP
