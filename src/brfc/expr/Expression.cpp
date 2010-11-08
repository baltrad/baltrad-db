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

#include <brfc/expr/Expression.hpp>

#include <brfc/String.hpp>
#include <brfc/Variant.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

namespace brfc {
namespace expr {

BinaryOperatorPtr
Expression::ne(const Expression& rhs) const {
    return make_shared<BinaryOperator>("!=", *this, rhs);
}

BinaryOperatorPtr
Expression::eq(const Expression& rhs) const {
    return make_shared<BinaryOperator>("=", *this, rhs);
}

BinaryOperatorPtr
Expression::gt(const Expression& rhs) const {
    return make_shared<BinaryOperator>(">", *this, rhs);
}

BinaryOperatorPtr
Expression::lt(const Expression& rhs) const {
    return make_shared<BinaryOperator>("<", *this, rhs);
}

BinaryOperatorPtr
Expression::le(const Expression& rhs) const {
    return make_shared<BinaryOperator>("<=", *this, rhs);
}

BinaryOperatorPtr
Expression::ge(const Expression& rhs) const {
    return make_shared<BinaryOperator>(">=", *this, rhs);
}

BinaryOperatorPtr
Expression::and_(const Expression& rhs) const {
    return make_shared<BinaryOperator>("AND", *this, rhs);
}

BinaryOperatorPtr
Expression::or_(const Expression& rhs) const {
    return make_shared<BinaryOperator>("OR", *this, rhs);
}

BinaryOperatorPtr
Expression::add(const Expression& rhs) const {
    return make_shared<BinaryOperator>("+", *this, rhs);
}

BinaryOperatorPtr
Expression::between(const Expression& low, const Expression& high) const {
    // this >= low and this <= high
    ExpressionPtr rhs = le(high);
    ExpressionPtr lhs = ge(low);
    return lhs->and_(*rhs);
}

BinaryOperatorPtr
Expression::like(const String& pattern) const {
    Variant v(pattern);
    Literal l(v);
    return like(l);
}

BinaryOperatorPtr
Expression::like(const Expression& pattern) const {
    return make_shared<BinaryOperator>("LIKE", *this, pattern);
}

ParenthesesPtr
Expression::parentheses() const {
    return make_shared<Parentheses>(*this);
}

LabelPtr
Expression::label(const String& label) const {
    return make_shared<Label>(*this, label);
}

} // namespace expr
} // namespace brfc
