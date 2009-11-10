#ifndef BRFC_EXPR_FWD_HPP
#define BRFC_EXPR_FWD_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

/**
 * @brief sql expressions
 */
namespace expr {

class Visitor;
class Alias;
class Attribute;
class BinaryOperator;
class Element;
class Expression;
class FromClause;
class Column;
class Label;
class Literal;
class Parentheses;
class Selectable;
class Select;
class Table;

typedef shared_ptr<Alias> AliasPtr;
typedef shared_ptr<Attribute> AttributePtr;
typedef shared_ptr<BinaryOperator> BinaryOperatorPtr;
typedef shared_ptr<Element> ElementPtr;
typedef shared_ptr<Expression> ExpressionPtr;
typedef shared_ptr<FromClause> FromClausePtr;
typedef shared_ptr<Column> ColumnPtr;
typedef shared_ptr<Label> LabelPtr;
typedef shared_ptr<Literal> LiteralPtr;
typedef shared_ptr<Parentheses> ParenthesesPtr;
typedef shared_ptr<Selectable> SelectablePtr;
typedef shared_ptr<Select> SelectPtr;
typedef shared_ptr<Table> TablePtr;

}
}

#endif // BRFC_EXPR_FWD_HPP
