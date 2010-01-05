#include <brfc/expr/Visitor.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Visitor.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Join.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Table.hpp>

#include <boost/foreach.hpp>

namespace brfc {
namespace expr {


void
Visitor::visit(Alias& alias) {
    if (order_ == PRE_ORDER)
        do_visit(alias);
    alias.aliased()->accept(*this);
    if (order_ == POST_ORDER)
        do_visit(alias);
}

void
Visitor::visit(Attribute& attribute) {
    do_visit(attribute);
}

void
Visitor::visit(Column& column) {
    if (order_ == PRE_ORDER)
        do_visit(column);
    column.selectable()->accept(*this);
    if (order_ == POST_ORDER)
        do_visit(column);
}

void
Visitor::visit(FromClause& from) {
    in_from_clause = true; // XXX: hack
    if (order_== PRE_ORDER)
        do_visit(from);
    BOOST_FOREACH(SelectablePtr element, from.elements()) {
        element->accept(*this);
    }
    if (order_== POST_ORDER)
        do_visit(from);
    in_from_clause = false; //XXX: hack
}

void
Visitor::visit(BinaryOperator& op) {
    if (order_== PRE_ORDER)
        do_visit(op);
    op.lhs()->accept(*this);
    op.rhs()->accept(*this);
    if (order_== POST_ORDER)
        do_visit(op);
}

void
Visitor::visit(Label& label) {
    if (order_== PRE_ORDER)
        do_visit(label);
    label.expression()->accept(*this);
    if (order_== POST_ORDER)
        do_visit(label);
}

void
Visitor::visit(Join& join) {
    if (order_ == PRE_ORDER)
        do_visit(join);
    join.from()->accept(*this);
    join.to()->accept(*this);
    join.condition()->accept(*this);
    if (order_ == POST_ORDER)
        do_visit(join);
}

void
Visitor::visit(Literal& literal) {
    do_visit(literal);
}

void
Visitor::visit(Parentheses& parentheses) {
    if (order_ == PRE_ORDER)
        do_visit(parentheses);
    parentheses.expression()->accept(*this);
    if (order_ == POST_ORDER)
        do_visit(parentheses);
}

void
Visitor::visit(Table& table) {
    do_visit(table);
}

void
Visitor::visit(Select& select) {
    if (order_ == PRE_ORDER)
        do_visit(select);
    
    BOOST_FOREACH(ExpressionPtr col, select.what()) {
        col->accept(*this);
    }

    select.from()->accept(*this);

    if (select.where())
        select.where()->accept(*this);

    if (order_ == POST_ORDER)
        do_visit(select);
}

} // namespace expr
} // namespace brfc
