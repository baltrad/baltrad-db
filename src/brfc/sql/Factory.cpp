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

#include <brfc/sql/Factory.hpp>

#include <brfc/DateTime.hpp>

#include <brfc/expr/Listcons.hpp>

using ::brfc::expr::Expression;
using ::brfc::expr::Listcons;

namespace brfc {
namespace sql {

Expression
Factory::literal(const Expression& value) const {
    return Listcons().symbol("lit").append(value).get();
}

Expression
Factory::string(const std::string& value) const {
    return Listcons().symbol("lit").string(value).get();
}

Expression
Factory::string(const char* value) const {
    return Listcons().symbol("lit").string(value).get();
}

Expression
Factory::int64_(long long value) const {
    return Listcons().symbol("lit").int64(value).get();
}

Expression
Factory::double_(double value) const {
    return Listcons().symbol("lit").double_(value).get();
}

Expression
Factory::date(const Date& value) const {
    return Listcons().symbol("lit").date(value).get();
}

Expression
Factory::time(const Time& value) const {
    return Listcons().symbol("lit").time(value).get();
}

Expression
Factory::datetime(const DateTime& value) const {
    return Listcons().symbol("lit").datetime(value).get();
}

Expression
Factory::bool_(bool value) const {
    return Listcons().symbol("lit").bool_(value).get();
}

Expression
Factory::bind(const std::string& name) const {
    return Listcons().symbol("bind").string(name).get();
}

Expression
Factory::ne(const Expression& lhs, const Expression& rhs) const {
    return binary_op("!=", lhs, rhs);
}

Expression
Factory::eq(const Expression& lhs, const Expression& rhs) const {
    return binary_op("=", lhs, rhs);
}

Expression
Factory::gt(const Expression& lhs, const Expression& rhs) const {
    return binary_op(">", lhs, rhs);
}

Expression
Factory::lt(const Expression& lhs, const Expression& rhs) const {
    return binary_op("<", lhs, rhs);
}

Expression
Factory::le(const Expression& lhs, const Expression& rhs) const {
    return binary_op("<=", lhs, rhs);
}

Expression
Factory::ge(const Expression& lhs, const Expression& rhs) const {
    return binary_op(">=", lhs, rhs);
}

Expression
Factory::between(const Expression& x,
                 const Expression& low,
                 const Expression& high) const {
    return and_(le(low, x), le(x, high));
}

Expression
Factory::and_(const Expression& lhs, const Expression& rhs) const {
    return binary_op("and", lhs, rhs);
}

Expression
Factory::or_(const Expression& lhs, const Expression& rhs) const {
    return binary_op("or", lhs, rhs);
}

Expression
Factory::not_(const Expression& exp) const {
    return unary_op("not", exp);
}

Expression
Factory::add(const Expression& lhs, const Expression& rhs) const {
    return binary_op("+", lhs, rhs);
}

Expression
Factory::binary_op(const std::string& op,
                   const Expression& lhs,
                   const Expression& rhs) const {
    return Listcons().symbol(op).append(lhs).append(rhs).get();
}

Expression
Factory::unary_op(const std::string& op,
                   const Expression& arg) const {
    return Listcons().symbol(op).append(arg).get();
}

Expression
Factory::alias(const expr::Expression& x, const std::string& alias) const {
    return Listcons().symbol("alias").append(x).string(alias).get();
}

Expression
Factory::label(const expr::Expression& x, const std::string& alias) const {
    return Listcons().symbol("label").append(x).string(alias).get();
}

Expression
Factory::min(const expr::Expression& x) const {
    return unary_op("min", x);
}

Expression
Factory::max(const expr::Expression& x) const {
    return unary_op("max", x);
}

Expression
Factory::sum(const expr::Expression& x) const {
    return unary_op("sum", x);
}

Expression
Factory::count(const expr::Expression& x) const {
    return unary_op("count", x);
}

Expression
Factory::column(const std::string& table, const std::string& column) const {
    return Listcons().symbol("column").string(table).string(column).get();
}

Expression
Factory::table(const std::string& table) const {
    return Listcons().symbol("table").string(table).get();
}
} // namespace sql
} // namespace brfc
