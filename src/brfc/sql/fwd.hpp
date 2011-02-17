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

#ifndef BRFC_SQL_FWD_HPP
#define BRFC_SQL_FWD_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

/**
 * @brief relational database connection & SQL
 */
namespace sql {

class Alias;
class Attribute;
class Bind;
class BinaryOperator;
class Column;
class Element;
class Expression;
class ExpressionList;
class Function;
class Insert;
class Join;
class Label;
class Literal;
class Parentheses;
class Selectable;
class Select;
class Table;

typedef shared_ptr<Alias> AliasPtr;
typedef shared_ptr<Attribute> AttributePtr;
typedef shared_ptr<Bind> BindPtr;
typedef shared_ptr<BinaryOperator> BinaryOperatorPtr;
typedef shared_ptr<Column> ColumnPtr;
typedef shared_ptr<Element> ElementPtr;
typedef shared_ptr<Expression> ExpressionPtr;
typedef shared_ptr<ExpressionList> ExpressionListPtr;
typedef shared_ptr<Function> FunctionPtr;
typedef shared_ptr<Insert> InsertPtr;
typedef shared_ptr<Join> JoinPtr;
typedef shared_ptr<Label> LabelPtr;
typedef shared_ptr<Literal> LiteralPtr;
typedef shared_ptr<Parentheses> ParenthesesPtr;
typedef shared_ptr<Selectable> SelectablePtr;
typedef shared_ptr<const Selectable> ConstSelectablePtr;
typedef shared_ptr<Select> SelectPtr;
typedef shared_ptr<Table> TablePtr;

}
}

#endif // BRFC_SQL_FWD_HPP
