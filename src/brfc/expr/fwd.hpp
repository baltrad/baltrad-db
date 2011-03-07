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

#ifndef BRFC_EXPR_FWD_HPP
#define BRFC_EXPR_FWD_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

/**
 * @brief query expressions
 */
namespace expr {

class Attribute;
class BinaryOperator;
class Expression;
class ExpressionList;
class Function;
class Label;
class Literal;
class Parentheses;

typedef shared_ptr<Attribute> AttributePtr;
typedef shared_ptr<BinaryOperator> BinaryOperatorPtr;
typedef shared_ptr<Expression> ExpressionPtr;
typedef shared_ptr<ExpressionList> ExpressionListPtr;
typedef shared_ptr<Function> FunctionPtr;
typedef shared_ptr<Literal> LiteralPtr;
typedef shared_ptr<Parentheses> ParenthesesPtr;

}
}

#endif // BRFC_EXPR_FWD_HPP
