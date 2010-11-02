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

#include <brfc/expr/ExpressionFactory.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Function.hpp>
#include <brfc/expr/Literal.hpp>

namespace brfc {
namespace expr {

ExpressionFactory::ExpressionFactory()
        : prototypes_(AttributePrototypes::default_odim_h5()) {

}

ExpressionFactory::ExpressionFactory(const AttributePrototypes& prototypes)
        : prototypes_(prototypes) {

}

AttributePtr
ExpressionFactory::attribute(const String& name) const {
    return prototypes_.get(name);
}

LiteralPtr
ExpressionFactory::string(const String& value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
ExpressionFactory::string(const char* value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
ExpressionFactory::int64_(long long value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
ExpressionFactory::double_(double value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
ExpressionFactory::date(int year, int month, int day) const {
    return date(Date(year, month, day));
}

LiteralPtr
ExpressionFactory::date(const Date& date) const {
    return Literal::create(Variant(Date(date)));
}

LiteralPtr
ExpressionFactory::date(const DateTime& datetime) const {
    return date(datetime.date());
}

LiteralPtr
ExpressionFactory::time(int hour, int minute, int second) const {
    return time(Time(hour, minute, second));
}

LiteralPtr
ExpressionFactory::time(const Time& time) const {
    return Literal::create(Variant(time));
}

LiteralPtr
ExpressionFactory::time(const DateTime& datetime) const {
    return time(datetime.time());
}

LiteralPtr
ExpressionFactory::datetime(const DateTime& datetime) const {
    return Literal::create(Variant(datetime));
}

LiteralPtr
ExpressionFactory::bool_(bool value) const {
    return Literal::create(Variant(value));
}

BinaryOperatorPtr
ExpressionFactory::ne(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->ne(rhs);
}

BinaryOperatorPtr
ExpressionFactory::eq(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->eq(rhs);
}

BinaryOperatorPtr
ExpressionFactory::gt(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->gt(rhs);
}

BinaryOperatorPtr
ExpressionFactory::lt(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->lt(rhs);
}

BinaryOperatorPtr
ExpressionFactory::le(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->le(rhs);
}

BinaryOperatorPtr
ExpressionFactory::ge(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->ge(rhs);
}

BinaryOperatorPtr
ExpressionFactory::between(ExpressionPtr expr,
                 ExpressionPtr low,
                 ExpressionPtr high) const {
    return expr->between(low, high);
}

BinaryOperatorPtr
ExpressionFactory::and_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->and_(rhs);
}

BinaryOperatorPtr
ExpressionFactory::or_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->or_(rhs);
}

BinaryOperatorPtr
ExpressionFactory::add(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->add(rhs);
}

ParenthesesPtr
ExpressionFactory::parentheses(ExpressionPtr expr) const {
    return expr->parentheses();
}

FunctionPtr
ExpressionFactory::min(ExpressionPtr expr) const {
    return Function::min(expr);
}


FunctionPtr
ExpressionFactory::max(ExpressionPtr expr) const {
    return Function::max(expr);
}

ExpressionPtr
ExpressionFactory::combined_datetime(const String& date,
                                     const String& time) const {
    AttributePtr date_attr = attribute(date);
    if (date_attr->type() != Attribute::DATE)
        throw value_error("invalid attribute for date");
    AttributePtr time_attr = attribute(time);
    if (time_attr->type() != Attribute::TIME)
        throw value_error("invalid attribute for time");
    return add(date_attr, time_attr);
}

} // namespace expr
} // namespace brfc
