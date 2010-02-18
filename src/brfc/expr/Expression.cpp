#include <brfc/expr/Expression.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

BinaryOperatorPtr
Expression::ne(ExpressionPtr rhs) const {
    return BinaryOperator::create("!=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::eq(ExpressionPtr rhs) const {
    return BinaryOperator::create("=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::gt(ExpressionPtr rhs) const {
    return BinaryOperator::create(">", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::lt(ExpressionPtr rhs) const {
    return BinaryOperator::create("<", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::le(ExpressionPtr rhs) const {
    return BinaryOperator::create("<=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::ge(ExpressionPtr rhs) const {
    return BinaryOperator::create(">=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::and_(ExpressionPtr rhs) const {
    return BinaryOperator::create("AND", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::or_(ExpressionPtr rhs) const {
    return BinaryOperator::create("OR", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::between(ExpressionPtr low, ExpressionPtr high) const {
    // this >= low and this <= high
    ExpressionPtr rhs = le(high);
    ExpressionPtr lhs = ge(low);
    return lhs->and_(rhs);
}

ParenthesesPtr
Expression::parentheses() const {
    return Parentheses::create(this->shared_from_this());
}

LabelPtr
Expression::label(const QString& label) const {
    return Label::create(this->shared_from_this(), label);
}

} // namespace expr
} // namespace brfc
