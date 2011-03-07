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

#include <brfc/expr/Expression.hpp>

#include <string>
#include <brfc/Variant.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionList.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

namespace brfc {
namespace expr {

BinaryOperatorPtr
Expression::ne(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::NE, *this, rhs);
}

BinaryOperatorPtr
Expression::eq(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::EQ, *this, rhs);
}

BinaryOperatorPtr
Expression::gt(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::GT, *this, rhs);
}

BinaryOperatorPtr
Expression::lt(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::LT, *this, rhs);
}

BinaryOperatorPtr
Expression::le(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::LE, *this, rhs);
}

BinaryOperatorPtr
Expression::ge(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::GE, *this, rhs);
}

BinaryOperatorPtr
Expression::and_(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::AND, *this, rhs);
}

BinaryOperatorPtr
Expression::or_(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::OR, *this, rhs);
}

BinaryOperatorPtr
Expression::add(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::ADD, *this, rhs);
}

BinaryOperatorPtr
Expression::sub(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::SUB, *this, rhs);
}

BinaryOperatorPtr
Expression::mul(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::MUL, *this, rhs);
}

BinaryOperatorPtr
Expression::div(const Expression& rhs) const {
    return make_shared<BinaryOperator>(BinaryOperator::DIV, *this, rhs);
}

BinaryOperatorPtr
Expression::between(const Expression& low, const Expression& high) const {
    // this >= low and this <= high
    ExpressionPtr rhs = le(high);
    ExpressionPtr lhs = ge(low);
    return lhs->and_(*rhs);
}

BinaryOperatorPtr
Expression::like(const std::string& pattern) const {
    Variant v(pattern);
    Literal l(v);
    return like(l);
}

BinaryOperatorPtr
Expression::like(const Expression& pattern) const {
    return make_shared<BinaryOperator>(BinaryOperator::LIKE, *this, pattern);
}

BinaryOperatorPtr
Expression::in(const ExpressionList& values) const {
    return make_shared<BinaryOperator>(BinaryOperator::IN, *this, values);
}

BinaryOperatorPtr
Expression::not_in(const ExpressionList& values) const {
    return make_shared<BinaryOperator>(BinaryOperator::NOT_IN, *this, values);
}

ParenthesesPtr
Expression::parentheses() const {
    return make_shared<Parentheses>(*this);
}

} // namespace expr
} // namespace brfc
