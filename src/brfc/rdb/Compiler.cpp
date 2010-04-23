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

#include <brfc/rdb/Compiler.hpp>

#include <algorithm>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/rdb/Alias.hpp>
#include <brfc/rdb/Column.hpp>
#include <brfc/rdb/FromClause.hpp>
#include <brfc/rdb/Join.hpp>
#include <brfc/rdb/Select.hpp>
#include <brfc/rdb/Table.hpp>

namespace brfc {
namespace rdb {

template<typename T>
void
Compiler::compile(T& expr) {
    visit(expr, *this);
}

// explicitly instantiate the template
template void Compiler::compile(Alias& expr);
template void Compiler::compile(Column& expr);
template void Compiler::compile(FromClause& expr);
template void Compiler::compile(Join& expr);
template void Compiler::compile(Select& expr);
template void Compiler::compile(Table& expr);
template void Compiler::compile(expr::Attribute& expr);
template void Compiler::compile(expr::BinaryOperator& expr);
template void Compiler::compile(expr::Expression& expr);
template void Compiler::compile(expr::Label& expr);
template void Compiler::compile(expr::Literal& expr);
template void Compiler::compile(expr::Parentheses& expr);

QString
Compiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    QString top = stack_.back();
    stack_.pop_back();
    return top;
}

void
Compiler::push(const QString& top) {
    stack_.push_back(top);
}

void
Compiler::operator()(expr::BinaryOperator& expr) {
    visit(*expr.lhs(), *this);
    visit(*expr.rhs(), *this);
    const QString& rhs = pop();
    const QString& lhs = pop();
    push(lhs + " " + expr.op() + " " + rhs);
}

void
Compiler::operator()(Column& expr) {
    visit(*expr.selectable(), *this);
    push(pop() + "." + expr.name());
}

void
Compiler::operator()(Alias& expr) {
    if (in_from_clause_) {
        visit(*expr.aliased(), *this);
        push(pop() + " AS " + expr.alias());
    } else {
        // discard aliased content
        push(expr.alias()); // replace with an alias instead
    }
}

void
Compiler::operator()(expr::Attribute& expr) {
    BRFC_ASSERT(false); // attributes should be replaced at this point
}

void
Compiler::operator()(Join& join) {
    in_from_clause_ = true;
    visit(*join.from(), *this);
    visit(*join.to(), *this);
    in_from_clause_ = false;
    visit(*join.condition(), * this);
    in_from_clause_ = true;

    QString condition = pop();
    QString to = pop();
    QString from = pop();

    QString jointype;
    switch (join.type()) {
        case Join::INNER:
            jointype = " JOIN ";
            break;
        case Join::LEFT:
            jointype = " LEFT JOIN ";
            break;
        default:
            BRFC_ASSERT(false);
    }

    push(from + jointype + to + " ON " + condition);
}

void
Compiler::operator()(expr::Literal& expr) {
    QString key = QString(":lit_") + QString::number(literal_count_++);
    push(key);
    binds_.add(key, expr.value());
}

void
Compiler::operator()(expr::Label& label) {
    visit(*label.expression(), *this);
    push(pop() + " AS " + label.name());
}

void
Compiler::operator()(expr::Parentheses& expr) {
    visit(*expr.expression(), *this);
    push("(" + pop() + ")");
}

void
Compiler::operator()(FromClause& from) {
    if (from.empty())
        return;

    in_from_clause_ = true;

    BOOST_FOREACH(SelectablePtr element, from.elements()) {
        visit(*element, *this);
    }

    QStringList from_clause_elements;
    for (size_t i = 0; i < from.elements().size(); ++i) {
        from_clause_elements.push_back(pop());
    }
    std::reverse(from_clause_elements.begin(), from_clause_elements.end());
    QString from_clause = "\nFROM " + from_clause_elements.join(", ");
    push(from_clause);

    in_from_clause_ = false;
}

void
Compiler::operator()(Select& select) {

    BOOST_FOREACH(expr::ExpressionPtr col, select.what()) {
        visit(*col, *this);
    }
    
    visit(*select.from(), *this);

    if (select.where())
        visit(*select.where(), *this);

    QString where_clause;
    if (select.where())
        where_clause = "\nWHERE " + pop();
    QString from_clause;
    if (!select.from()->empty())
        from_clause = pop();
    
    QStringList result_column_elm;
    for (size_t i = 0; i < select.what().size(); ++i) {
        result_column_elm.push_back(pop());
    }
    std::reverse(result_column_elm.begin(), result_column_elm.end());
    QString result_columns = result_column_elm.join(", ");

    QString distinct = select.distinct() ? "DISTINCT " : "";
    // SELECT columns FROM from_obj WHERE where_clause
    QString clause = "SELECT " + distinct
                               + result_columns
                               + from_clause
                               + where_clause;
    push(clause);
}

void
Compiler::operator()(Table& expr) {
    push(expr.name());
}

} // namespace rdb
} // namespace brfc
