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

#include <brfc/sql/DialectCompiler.hpp>

#include <algorithm>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/StringList.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Function.hpp>
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
DialectCompiler::do_compile(const Element& expr) {
    stack_.clear();
    binds_.clear();
    visit(expr, *this);
    return Query(stack_.back(), binds_);
}

String
DialectCompiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    String top = stack_.back();
    stack_.pop_back();
    return top;
}

void
DialectCompiler::push(const String& top) {
    stack_.push_back(top);
}

void
DialectCompiler::operator()(const BinaryOperator& expr) {
    visit(*expr.lhs(), *this);
    visit(*expr.rhs(), *this);
    const String& rhs = pop();
    const String& lhs = pop();
    push(lhs + " " + expr.op() + " " + rhs);
}

void
DialectCompiler::operator()(const Column& expr) {
    visit(*expr.selectable(), *this);
    push(pop() + "." + expr.name());
}

void
DialectCompiler::operator()(const Function& func) {
    StringList args;
    BOOST_FOREACH(ExpressionPtr arg, func.args()) {
        visit(*arg, *this);
        args.append(pop());
    }
    push(func.name() + "(" + args.join(", ") + ")");
}

void
DialectCompiler::operator()(const Alias& expr) {
    if (in_from_clause_) {
        visit(*expr.aliased(), *this);
        push(pop() + " AS " + expr.alias());
    } else {
        // discard aliased content
        push(expr.alias()); // replace with an alias instead
    }
}

void
DialectCompiler::operator()(const Join& join) {
    String condition, lhs, rhs, jointype;
    in_from_clause_ = true;
    visit(*join.rhs(), *this);
    visit(*join.lhs(), *this);
    in_from_clause_ = false;
    if (join.condition()) {
        visit(*join.condition(), * this);
        condition = pop();
    }
    in_from_clause_ = true;

    lhs = pop();
    rhs = pop();

    switch (join.type()) {
        case Join::CROSS:
            jointype = " CROSS JOIN ";
            break;
        case Join::INNER:
            jointype = " JOIN ";
            break;
        case Join::LEFT:
            jointype = " LEFT JOIN ";
            break;
        default:
            BRFC_ASSERT(false);
    }

    if (join.condition())
        push(lhs + jointype + rhs + " ON " + condition);
    else {
        push(lhs + jointype + rhs);
    }
}

void
DialectCompiler::operator()(const Literal& expr) {
    push(dialect_->variant_to_string(expr.value()));
}

void
DialectCompiler::operator()(const Label& label) {
    visit(*label.expression(), *this);
    push(pop() + " AS " + label.name());
}

void
DialectCompiler::operator()(const Parentheses& expr) {
    visit(*expr.expression(), *this);
    push("(" + pop() + ")");
}

void
DialectCompiler::operator()(const Select& select) {

    BOOST_FOREACH(ExpressionPtr col, select.what()) {
        visit(*col, *this);
    }
    
    if (select.where())
        visit(*select.where(), *this);

    String where_clause;
    if (select.where())
        where_clause = "\nWHERE " + pop();

    String from_clause;
    if (select.from()) {
        in_from_clause_ = true;
        visit(*select.from(), *this);
        in_from_clause_ = false;
        from_clause = "\nFROM " + pop();
    }

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
DialectCompiler::operator()(const Insert& insert) {
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
DialectCompiler::operator()(const Table& expr) {
    push(expr.name());
}

} // namespace sql
} // namespace brfc
