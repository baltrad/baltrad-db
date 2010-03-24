#include <brfc/expr/Factory.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Literal.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

namespace brfc {
namespace expr {

AttributePtr
Factory::attribute(const QString& name) const {
    return Attribute::create(name);
}

LiteralPtr
Factory::string(const QString& value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::string(const char* value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::integer(long long value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::real(double value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::date(int year, int month, int day) const {
    return Literal::create(Variant(QDate(year, month, day)));
}

LiteralPtr
Factory::time(int hour, int minute, int second) const {
    return Literal::create(Variant(QTime(hour, minute, second)));
}

LiteralPtr
Factory::boolean(bool value) const {
    return Literal::create(Variant(value));
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
    return lhs->or_(rhs);
}

ParenthesesPtr
Factory::parentheses(ExpressionPtr xpr) const {
    return xpr->parentheses();
}

} // namespace expr
} // namespace brfc
