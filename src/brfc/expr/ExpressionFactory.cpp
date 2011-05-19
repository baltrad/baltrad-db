/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/Variant.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {

ExpressionFactory::ExpressionFactory()
        : prototypes_(AttributePrototypes::default_odim_h5()) {

}

ExpressionFactory::ExpressionFactory(const AttributePrototypes& prototypes)
        : prototypes_(prototypes) {

}

Expression
ExpressionFactory::attribute(const std::string& name) const {
    const std::string& type = prototypes_.typename_(name);
    return attribute(name, type);
}

Expression
ExpressionFactory::attribute(const std::string& name,
                             const std::string& type) const {
    return Listcons().symbol("attr").string(name).string(type).get();
}

Expression
ExpressionFactory::literal(const Variant& value) const {
    return Expression();
}

Expression
ExpressionFactory::string(const std::string& value) const {
    return Expression(value);
}

Expression
ExpressionFactory::string(const char* value) const {
    return Expression(value);
}

Expression
ExpressionFactory::int64_(long long value) const {
    return Expression(value);
}

Expression
ExpressionFactory::long_(long long value) const {
    return Expression(value);
}

Expression
ExpressionFactory::double_(double value) const {
    return Expression(value);
}

Expression
ExpressionFactory::date(int year, int month, int day) const {
    return date(Date(year, month, day));
}

Expression
ExpressionFactory::date(const Date& date) const {
    return Expression(date);
}

Expression
ExpressionFactory::date(const DateTime& datetime) const {
    return date(datetime.date());
}

Expression
ExpressionFactory::time(int hour, int minute, int second) const {
    return time(Time(hour, minute, second));
}

Expression
ExpressionFactory::time(const Time& time) const {
    return Expression(time);
}

Expression
ExpressionFactory::time(const DateTime& datetime) const {
    return time(datetime.time());
}

Expression
ExpressionFactory::datetime(const DateTime& datetime) const {
    return Expression(datetime);
}

Expression
ExpressionFactory::timedelta(const TimeDelta& delta) const {
    return Expression(delta);
}

Expression
ExpressionFactory::bool_(bool value) const {
    return Expression(value);
}

Expression
ExpressionFactory::list(const Expression& x) const {
    return Listcons().symbol("list").extend(x).get();
}

Expression
ExpressionFactory::unary_op(const std::string& op, const Expression& arg) const {
    return Listcons().symbol(op).append(arg).get();
}

Expression
ExpressionFactory::binary_op(const std::string& op,
                             const Expression& lhs,
                             const Expression& rhs) const {
    return Listcons().symbol(op).append(lhs).append(rhs).get();
}

Expression
ExpressionFactory::ne(const Expression& lhs, const Expression& rhs) const {
    return binary_op("!=", lhs, rhs);
}

Expression
ExpressionFactory::eq(const Expression& lhs, const Expression& rhs) const {
    return binary_op("=", lhs, rhs);
}

Expression
ExpressionFactory::gt(const Expression& lhs, const Expression& rhs) const {
    return binary_op(">", lhs, rhs);
}

Expression
ExpressionFactory::lt(const Expression& lhs, const Expression& rhs) const {
    return binary_op("<", lhs, rhs);
}

Expression
ExpressionFactory::le(const Expression& lhs, const Expression& rhs) const {
    return binary_op("<=", lhs, rhs);
}

Expression
ExpressionFactory::ge(const Expression& lhs, const Expression& rhs) const {
    return binary_op(">=", lhs, rhs);
}

Expression
ExpressionFactory::between(const Expression& expr,
                           const Expression& low,
                           const Expression& high) const {
    return and_(le(low, expr), le(expr, high));
}

Expression
ExpressionFactory::and_(const Expression& lhs, const Expression& rhs) const {
    return binary_op("and", lhs, rhs);
}

Expression
ExpressionFactory::and_(const Expression& x) const {
    BRFC_ASSERT(x.is_list());
    if (x.size() < 1)
        throw value_error("Expression must have at least 1 element");
    Expression::const_iterator it = x.begin();
    Expression result = *it;
    ++it;
    for ( ; it != x.end(); ++it) {
        result = and_(result, *it);
    }
    return result;
}

Expression
ExpressionFactory::or_(const Expression& lhs, const Expression& rhs) const {
    return binary_op("or", lhs, rhs);
}

Expression
ExpressionFactory::or_(const Expression& x) const {
    BRFC_ASSERT(x.is_list());
    if (x.size() < 1)
        throw value_error("Expression must have at least 1 element");
    Expression::const_iterator it = x.begin();
    Expression result = *it;
    ++it;
    for ( ; it != x.end(); ++it) {
        result = or_(result, *it);
    }
    return result;
}

Expression
ExpressionFactory::not_(const Expression& x) const {
    return unary_op("not", x);
}

Expression
ExpressionFactory::add(const Expression& lhs, const Expression& rhs) const {
    return binary_op("+", lhs, rhs);
}

Expression
ExpressionFactory::sub(const Expression& lhs, const Expression& rhs) const {
    return binary_op("-", lhs, rhs);
}

Expression
ExpressionFactory::mul(const Expression& lhs, const Expression& rhs) const {
    return binary_op("*", lhs, rhs);
}

Expression
ExpressionFactory::div(const Expression& lhs, const Expression& rhs) const {
    return binary_op("/", lhs, rhs);
}

Expression
ExpressionFactory::parentheses(const Expression& x) const {
    return x;
}

Expression
ExpressionFactory::like(const Expression& x, const std::string& pattern) const {
    return binary_op("like", x, Expression(pattern));
}

Expression
ExpressionFactory::in(const Expression& x, const Expression& l) const {
    return binary_op("in", x, l);
}

Expression
ExpressionFactory::min(const Expression& x) const {
    return unary_op("min", x);
}

Expression
ExpressionFactory::max(const Expression& x) const {
    return unary_op("max", x);
}

Expression
ExpressionFactory::sum(const Expression& x) const {
    return unary_op("sum", x);
}

Expression
ExpressionFactory::count(const Expression& x) const {
    return unary_op("count", x);
}

Expression
ExpressionFactory::combined_datetime(const std::string& date,
                                     const std::string& time) const {
    return add(attribute(date), attribute(time));
}

} // namespace brfc
