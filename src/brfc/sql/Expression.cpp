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

#include <brfc/sql/Expression.hpp>

#include <string>

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/ExpressionList.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Parentheses.hpp>

namespace brfc {
namespace sql {

const std::string&
Expression::name() const {
    static std::string empty;
    return empty;
}

BinaryOperatorPtr
Expression::ne(ExpressionPtr rhs) const {
    return BinaryOperator::create("!=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::eq(ExpressionPtr rhs) const {
    return BinaryOperator::create("=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::gt(ExpressionPtr rhs) const {
    return BinaryOperator::create(">", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::lt(ExpressionPtr rhs) const {
    return BinaryOperator::create("<", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::le(ExpressionPtr rhs) const {
    return BinaryOperator::create("<=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::ge(ExpressionPtr rhs) const {
    return BinaryOperator::create(">=", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::and_(ExpressionPtr rhs) const {
    return BinaryOperator::create("AND", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::or_(ExpressionPtr rhs) const {
    return BinaryOperator::create("OR", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::not_() const {
    return BinaryOperator::create("NOT", ExpressionPtr(), this->shared_from_this());
}

BinaryOperatorPtr
Expression::add(ExpressionPtr rhs) const {
    return BinaryOperator::create("+", this->shared_from_this(), rhs);
}

BinaryOperatorPtr
Expression::between(ExpressionPtr low, ExpressionPtr high) const {
    // this >= low and this <= high
    ExpressionPtr rhs = le(high);
    ExpressionPtr lhs = ge(low);
    return lhs->and_(rhs);
}

BinaryOperatorPtr
Expression::in(ExpressionListPtr values) const {
    return BinaryOperator::create("IN", this->shared_from_this(), values->parentheses());
}

BinaryOperatorPtr
Expression::not_in(ExpressionListPtr values) const {
    return BinaryOperator::create("NOT IN", this->shared_from_this(), values->parentheses());
}

ParenthesesPtr
Expression::parentheses() const {
    return Parentheses::create(this->shared_from_this());
}

LabelPtr
Expression::label(const std::string& label) const {
    return Label::create(this->shared_from_this(), label);
}

} // namespace sql
} // namespace brfc
