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

#include <brfc/sql/Compiler.hpp>

#include <algorithm>

#include <boost/foreach.hpp>

#include <brfc/StringList.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/FromClause.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace sql {

Query
Compiler::compile(const Element& expr) {
    visit(expr, *this);
    return Query(stack_.back(), binds_);
}

String
Compiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    String top = stack_.back();
    stack_.pop_back();
    return top;
}

void
Compiler::push(const String& top) {
    stack_.push_back(top);
}

void
Compiler::operator()(const BinaryOperator& expr) {
    visit(*expr.lhs(), *this);
    visit(*expr.rhs(), *this);
    const String& rhs = pop();
    const String& lhs = pop();
    push(lhs + " " + expr.op() + " " + rhs);
}

void
Compiler::operator()(const Column& expr) {
    visit(*expr.selectable(), *this);
    push(pop() + "." + expr.name());
}

void
Compiler::operator()(const Alias& expr) {
    if (in_from_clause_) {
        visit(*expr.aliased(), *this);
        push(pop() + " AS " + expr.alias());
    } else {
        // discard aliased content
        push(expr.alias()); // replace with an alias instead
    }
}

void
Compiler::operator()(const Join& join) {
    in_from_clause_ = true;
    visit(*join.from(), *this);
    visit(*join.to(), *this);
    in_from_clause_ = false;
    visit(*join.condition(), * this);
    in_from_clause_ = true;

    String condition = pop();
    String to = pop();
    String from = pop();

    String jointype;
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
Compiler::operator()(const Literal& expr) {
    String key = String(":lit_") + String::number(literal_count_++);
    push(key);
    binds_.add(key, expr.value());
}

void
Compiler::operator()(const Label& label) {
    visit(*label.expression(), *this);
    push(pop() + " AS " + label.name());
}

void
Compiler::operator()(const Parentheses& expr) {
    visit(*expr.expression(), *this);
    push("(" + pop() + ")");
}

void
Compiler::operator()(const FromClause& from) {
    if (from.empty())
        return;

    in_from_clause_ = true;

    BOOST_FOREACH(ConstSelectablePtr element, from.elements()) {
        visit(*element, *this);
    }

    StringList from_clause_elements;
    for (size_t i = 0; i < from.elements().size(); ++i) {
        from_clause_elements.push_back(pop());
    }
    std::reverse(from_clause_elements.begin(), from_clause_elements.end());
    String from_clause = "\nFROM " + from_clause_elements.join(", ");
    push(from_clause);

    in_from_clause_ = false;
}

void
Compiler::operator()(const Select& select) {

    BOOST_FOREACH(ExpressionPtr col, select.what()) {
        visit(*col, *this);
    }
    
    visit(*select.from(), *this);

    if (select.where())
        visit(*select.where(), *this);

    String where_clause;
    if (select.where())
        where_clause = "\nWHERE " + pop();
    String from_clause;
    if (!select.from()->empty())
        from_clause = pop();
    
    StringList result_column_elm;
    for (size_t i = 0; i < select.what().size(); ++i) {
        result_column_elm.push_back(pop());
    }
    std::reverse(result_column_elm.begin(), result_column_elm.end());
    String result_columns = result_column_elm.join(", ");

    String distinct = select.distinct() ? "DISTINCT " : "";
    // SELECT columns FROM from_obj WHERE where_clause
    String clause = "SELECT " + distinct
                              + result_columns
                              + from_clause
                              + where_clause;
    push(clause);
}

void
Compiler::operator()(const Insert& insert) {
    StringList cols;
    StringList vals;
    BOOST_FOREACH(const Insert::ValueMap::value_type& bind, insert.values()) {
        cols.append(bind.first->name());
        visit(*bind.second, *this);
        vals.append(pop());
    }

    String stmt = "INSERT INTO " + insert.table()->name() + "(" +
                  cols.join(", ") + ") VALUES (" + vals.join(", ") + ")";
    
    if (insert.returns().size() > 0) {
        StringList rets;
        BOOST_FOREACH(ExpressionPtr expr, insert.returns()) {
            visit(*expr, *this);
            rets.append(pop());
        }
        stmt += " RETURNING " + rets.join(", ");
    }
    push(stmt);
}

void
Compiler::operator()(const Table& expr) {
    push(expr.name());
}

} // namespace sql
} // namespace brfc
