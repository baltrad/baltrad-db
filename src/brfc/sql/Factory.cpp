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

#include <brfc/sql/Factory.hpp>

#include <brfc/DateTime.hpp>

#include <brfc/sql/Expression.hpp>
#include <brfc/sql/Literal.hpp>

namespace brfc {
namespace sql {

LiteralPtr
Factory::string(const String& value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::string(const char* value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::integer(long long value) const {
    return int64_(value);
}

LiteralPtr
Factory::int64_(long long value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::real(double value) const {
    return double_(value);
}

LiteralPtr
Factory::double_(double value) const {
    return Literal::create(Variant(value));
}

LiteralPtr
Factory::date(int year, int month, int day) const {
    return date(Date(year, month, day));
}

LiteralPtr
Factory::date(const Date& date) const {
    return Literal::create(Variant(Date(date)));
}

LiteralPtr
Factory::date(const DateTime& datetime) const {
    return date(datetime.date());
}

LiteralPtr
Factory::time(int hour, int minute, int second) const {
    return time(Time(hour, minute, second));
}

LiteralPtr
Factory::time(const Time& time) const {
    return Literal::create(Variant(time));
}

LiteralPtr
Factory::time(const DateTime& datetime) const {
    return time(datetime.time());
}

LiteralPtr
Factory::datetime(const DateTime& datetime) const {
    return Literal::create(Variant(datetime));
}

LiteralPtr
Factory::boolean(bool value) const {
    return bool_(value);
}

LiteralPtr
Factory::bool_(bool value) const {
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
Factory::between(ExpressionPtr sql,
                 ExpressionPtr low,
                 ExpressionPtr high) const {
    return sql->between(low, high);
}

BinaryOperatorPtr
Factory::and_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->and_(rhs);
}

BinaryOperatorPtr
Factory::or_(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->or_(rhs);
}

BinaryOperatorPtr
Factory::add(ExpressionPtr lhs, ExpressionPtr rhs) const {
    return lhs->add(rhs);
}

ParenthesesPtr
Factory::parentheses(ExpressionPtr xpr) const {
    return xpr->parentheses();
}

} // namespace sql
} // namespace brfc
