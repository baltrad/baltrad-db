#ifndef BRFC_EXPR_FACTORY_HPP
#define BRFC_EXPR_FACTORY_HPP

#include <brfc/expr/fwd.hpp>
#include <string>

namespace brfc {
namespace expr {

/**
 * @brief factory for constructing expression elements
 * @ingroup exposed_in_binds
 */
class Factory {
  public:
    /**
     * @brief construct expr::Attribute
     */
    AttributePtr attribute(const std::string& name) const;

    /**
     * @brief construct expr::Literal containing a string
     *
     * the value is assumed to be encoded in UTF-8
     */
    LiteralPtr string(const std::string& value) const;

    /**
     * @brief construct expr::Literal containing a 64-bit integer
     */
    LiteralPtr integer(int64_t value) const;

    /**
     * @brief construct expr::Literal containing a double precision float
     */
    LiteralPtr real(double value) const;

    /**
     * @brief construct expr::Literal containing a date
     */
    LiteralPtr date(int year, int month, int day) const;

    /**
     * @brief construct expr::Literal containing a time
     */
    LiteralPtr time(int hour, int minute, int second=0) const;

    /**
     * @brief construct expr::Literal containing a bool
     */
    LiteralPtr boolean(bool value) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    BinaryOperatorPtr ne(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs == rhs
     */
    BinaryOperatorPtr eq(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs > rhs
     */
    BinaryOperatorPtr gt(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs < rhs
     */
    BinaryOperatorPtr lt(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs <= rhs
     */
    BinaryOperatorPtr le(ExpressionPtr lhs, ExpressionPtr rhs) const;

    /**
     * @brief lhs >= rhs
     */
    BinaryOperatorPtr ge(ExpressionPtr lhs, ExpressionPtr rhs) const;
    
    /**
     * @brief low <= expr => high
     */
    BinaryOperatorPtr between(ExpressionPtr expr,
                              ExpressionPtr low,
                              ExpressionPtr high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    BinaryOperatorPtr and_(ExpressionPtr lhs, ExpressionPtr rhs) const;
    
    /**
     * @brief lhs OR rhs
     */
    BinaryOperatorPtr or_(ExpressionPtr lhs, ExpressionPtr rhs) const;
    //@}

    /**
     * @brief surround expr with parentheses
     */
    ParenthesesPtr parentheses(ExpressionPtr xpr) const;
};

}
}

#endif // BRFC_EXPR_FACTORY_HPP
