#include <brfc/expr/Factory.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Literal.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

namespace brfc {
namespace expr {

AttributePtr
Factory::attribute(const std::string& name) const {
    return Attribute::create(name);
}

LiteralPtr
Factory::string(const std::string& value) const {
    return Literal::create(QString::fromUtf8(value.c_str()));
}

LiteralPtr
Factory::integer(long long value) const {
    return Literal::create(value);
}

LiteralPtr
Factory::real(double value) const {
    return Literal::create(value);
}

LiteralPtr
Factory::date(int year, int month, int day) const {
    return Literal::create(QDate(year, month, day));
}

LiteralPtr
Factory::time(int hour, int minute, int second) const {
    return Literal::create(QTime(hour, minute, second));
}

LiteralPtr
Factory::boolean(bool value) const {
    return Literal::create(value);
}

BinaryOperatorPtr
Factory::ne(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->ne(rhs);
}

BinaryOperatorPtr
Factory::eq(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->eq(rhs);
}

BinaryOperatorPtr
Factory::gt(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->gt(rhs);
}

BinaryOperatorPtr
Factory::lt(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->lt(rhs);
}

BinaryOperatorPtr
Factory::le(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->le(rhs);
}

BinaryOperatorPtr
Factory::ge(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->ge(rhs);
}

BinaryOperatorPtr
Factory::between(ExpressionPtr expr,
                          ExpressionPtr low,
                          ExpressionPtr high) const {
    return expr->between(low, high);
}

BinaryOperatorPtr
Factory::and_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->and_(rhs);
}

BinaryOperatorPtr
Factory::or_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return rhs->or_(rhs);
}

ParenthesesPtr
Factory::parentheses(ExpressionPtr xpr) const {
    return xpr->parentheses();
}

} // namespace expr
} // namespace brfc
