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
ExpressionFactory::attribute(const std::string& name) const {
    return static_pointer_cast<Attribute>(prototypes_.get(name).clone());
}

LiteralPtr
ExpressionFactory::string(const std::string& value) const {
    return make_shared<Literal>(Variant(value));
}

LiteralPtr
ExpressionFactory::string(const char* value) const {
    return make_shared<Literal>(Variant(value));
}

LiteralPtr
ExpressionFactory::int64_(long long value) const {
    return make_shared<Literal>(Variant(value));
}

LiteralPtr
ExpressionFactory::double_(double value) const {
    return make_shared<Literal>(Variant(value));
}

LiteralPtr
ExpressionFactory::date(int year, int month, int day) const {
    return date(Date(year, month, day));
}

LiteralPtr
ExpressionFactory::date(const Date& date) const {
    return make_shared<Literal>(Variant(date));
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
    return make_shared<Literal>(Variant(time));
}

LiteralPtr
ExpressionFactory::time(const DateTime& datetime) const {
    return time(datetime.time());
}

LiteralPtr
ExpressionFactory::datetime(const DateTime& datetime) const {
    return make_shared<Literal>(Variant(datetime));
}

LiteralPtr
ExpressionFactory::bool_(bool value) const {
    return make_shared<Literal>(Variant(value));
}

BinaryOperatorPtr
ExpressionFactory::ne(const Expression& lhs, const Expression& rhs) const {
    return lhs.ne(rhs);
}

BinaryOperatorPtr
ExpressionFactory::eq(const Expression& lhs, const Expression& rhs) const {
    return lhs.eq(rhs);
}

BinaryOperatorPtr
ExpressionFactory::gt(const Expression& lhs, const Expression& rhs) const {
    return lhs.gt(rhs);
}

BinaryOperatorPtr
ExpressionFactory::lt(const Expression& lhs, const Expression& rhs) const {
    return lhs.lt(rhs);
}

BinaryOperatorPtr
ExpressionFactory::le(const Expression& lhs, const Expression& rhs) const {
    return lhs.le(rhs);
}

BinaryOperatorPtr
ExpressionFactory::ge(const Expression& lhs, const Expression& rhs) const {
    return lhs.ge(rhs);
}

BinaryOperatorPtr
ExpressionFactory::between(const Expression& expr,
                           const Expression& low,
                           const Expression& high) const {
    return expr.between(low, high);
}

BinaryOperatorPtr
ExpressionFactory::and_(const Expression& lhs, const Expression& rhs) const {
    return lhs.and_(rhs);
}

BinaryOperatorPtr
ExpressionFactory::or_(const Expression& lhs, const Expression& rhs) const {
    return lhs.or_(rhs);
}

BinaryOperatorPtr
ExpressionFactory::add(const Expression& lhs, const Expression& rhs) const {
    return lhs.add(rhs);
}

ParenthesesPtr
ExpressionFactory::parentheses(const Expression& expr) const {
    return expr.parentheses();
}

FunctionPtr
ExpressionFactory::min(const Expression& expr) const {
    return Function::min(expr);
}


FunctionPtr
ExpressionFactory::max(const Expression& expr) const {
    return Function::max(expr);
}

ExpressionPtr
ExpressionFactory::combined_datetime(const std::string& date,
                                     const std::string& time) const {
    AttributePtr date_attr = attribute(date);
    if (date_attr->type() != Attribute::DATE)
        throw value_error("invalid attribute for date");
    AttributePtr time_attr = attribute(time);
    if (time_attr->type() != Attribute::TIME)
        throw value_error("invalid attribute for time");
    return add(*date_attr, *time_attr);
}

} // namespace expr
} // namespace brfc
