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
#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace expr {

ExpressionFactory::ExpressionFactory()
        : prototypes_(AttributePrototypes::default_odim_h5()) {

}

ExpressionFactory::ExpressionFactory(const AttributePrototypes& prototypes)
        : prototypes_(prototypes) {

}

sexp
ExpressionFactory::attribute(const std::string& name) const {
    const std::string& type = prototypes_.typename_(name);
    return listcons().symbol("attr").string(name).string(type).get();
}

sexp
ExpressionFactory::literal(const Variant& value) const {
    return sexp();
}

sexp
ExpressionFactory::string(const std::string& value) const {
    return sexp(value);
}

sexp
ExpressionFactory::string(const char* value) const {
    return sexp(value);
}

sexp
ExpressionFactory::int64_(long long value) const {
    return sexp(value);
}

sexp
ExpressionFactory::long_(long long value) const {
    return sexp(value);
}

sexp
ExpressionFactory::double_(double value) const {
    return sexp(value);
}

sexp
ExpressionFactory::date(int year, int month, int day) const {
    return date(Date(year, month, day));
}

sexp
ExpressionFactory::date(const Date& date) const {
    return sexp(date);
}

sexp
ExpressionFactory::date(const DateTime& datetime) const {
    return date(datetime.date());
}

sexp
ExpressionFactory::time(int hour, int minute, int second) const {
    return time(Time(hour, minute, second));
}

sexp
ExpressionFactory::time(const Time& time) const {
    return sexp(time);
}

sexp
ExpressionFactory::time(const DateTime& datetime) const {
    return time(datetime.time());
}

sexp
ExpressionFactory::datetime(const DateTime& datetime) const {
    return sexp(datetime);
}

sexp
ExpressionFactory::timedelta(const TimeDelta& delta) const {
    return sexp(delta);
}

sexp
ExpressionFactory::bool_(bool value) const {
    return sexp(value);
}

sexp
ExpressionFactory::unary(const std::string& op, const sexp& arg) const {
    return listcons().symbol(op).append(arg).get();
}

sexp
ExpressionFactory::binary(const std::string& op,
                         const sexp& lhs,
                         const sexp& rhs) const {
    return listcons().symbol(op).append(lhs).append(rhs).get();
}

sexp
ExpressionFactory::ne(const sexp& lhs, const sexp& rhs) const {
    return binary("!=", lhs, rhs);
}

sexp
ExpressionFactory::eq(const sexp& lhs, const sexp& rhs) const {
    return binary("=", lhs, rhs);
}

sexp
ExpressionFactory::gt(const sexp& lhs, const sexp& rhs) const {
    return binary(">", lhs, rhs);
}

sexp
ExpressionFactory::lt(const sexp& lhs, const sexp& rhs) const {
    return binary("<", lhs, rhs);
}

sexp
ExpressionFactory::le(const sexp& lhs, const sexp& rhs) const {
    return binary("<=", lhs, rhs);
}

sexp
ExpressionFactory::ge(const sexp& lhs, const sexp& rhs) const {
    return binary(">=", lhs, rhs);
}

sexp
ExpressionFactory::between(const sexp& expr,
                           const sexp& low,
                           const sexp& high) const {
    return and_(binary("<=", low, expr), binary("<=", expr, high));
}

sexp
ExpressionFactory::and_(const sexp& lhs, const sexp& rhs) const {
    return binary("and", lhs, rhs);
}

sexp
ExpressionFactory::and_(const sexp& x) const {
    BRFC_ASSERT(x.is_list());
    if (x.size() < 1)
        throw value_error("sexp must have at least 1 element");
    sexp::const_iterator it = x.begin();
    sexp result = *it;
    ++it;
    for ( ; it != x.end(); ++it) {
        result = and_(result, *it);
    }
    return result;
}

sexp
ExpressionFactory::or_(const sexp& lhs, const sexp& rhs) const {
    return binary("or", lhs, rhs);
}

sexp
ExpressionFactory::or_(const sexp& x) const {
    BRFC_ASSERT(x.is_list());
    if (x.size() < 1)
        throw value_error("sexp must have at least 1 element");
    sexp::const_iterator it = x.begin();
    sexp result = *it;
    ++it;
    for ( ; it != x.end(); ++it) {
        result = or_(result, *it);
    }
    return result;
}

sexp
ExpressionFactory::add(const sexp& lhs, const sexp& rhs) const {
    return binary("+", lhs, rhs);
}

sexp
ExpressionFactory::sub(const sexp& lhs, const sexp& rhs) const {
    return binary("-", lhs, rhs);
}

sexp
ExpressionFactory::mul(const sexp& lhs, const sexp& rhs) const {
    return binary("*", lhs, rhs);
}

sexp
ExpressionFactory::div(const sexp& lhs, const sexp& rhs) const {
    return binary("/", lhs, rhs);
}

sexp
ExpressionFactory::parentheses(const sexp& x) const {
    return x;
}

sexp
ExpressionFactory::like(const sexp& x, const std::string& pattern) const {
    return binary("like", x, sexp(pattern));
}

sexp
ExpressionFactory::in(const sexp& x, const sexp& l) const {
    return binary("in", x, l);
}

sexp
ExpressionFactory::not_in(const sexp& x, const sexp& l) const {
    return binary("not_in", x, l);
}

sexp
ExpressionFactory::min(const sexp& x) const {
    return unary("min", x);
}

sexp
ExpressionFactory::max(const sexp& x) const {
    return unary("max", x);
}

sexp
ExpressionFactory::sum(const sexp& x) const {
    return unary("sum", x);
}

sexp
ExpressionFactory::count(const sexp& x) const {
    return unary("count", x);
}

sexp
ExpressionFactory::combined_datetime(const std::string& date,
                                     const std::string& time) const {
    return add(attribute(date), attribute(time));
}

} // namespace expr
} // namespace brfc
