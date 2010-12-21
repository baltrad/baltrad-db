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

#include <sstream>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <brfc/assert.hpp>
#include <brfc/StringList.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Bind.hpp>
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
    BRFC_ASSERT(!stack_.empty());
    return Query(stack_.back(), binds_);
}

std::string
DialectCompiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    std::string top = stack_.back();
    stack_.pop_back();
    return top;
}

void
DialectCompiler::push(const std::string& top) {
    stack_.push_back(top);
}

void
DialectCompiler::operator()(const BinaryOperator& expr) {
    visit(*expr.lhs(), *this);
    visit(*expr.rhs(), *this);
    const std::string& rhs = pop();
    const std::string& lhs = pop();
    push(lhs + " " + expr.op() + " " + rhs);
}

void
DialectCompiler::operator()(const Bind& bind) {
    std::string name = bind.name();
    if (not boost::starts_with(name, ":"))
        name = ":" + name;
    binds_.add(name, Variant());
    push(name);
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
        bool is_select = dynamic_pointer_cast<Select>(expr.aliased());
        visit(*expr.aliased(), *this);
        std::stringstream ss;
        if (is_select) ss << "(";
        ss << pop();
        if (is_select) ss << ")";
        ss << " AS " << expr.alias();
        push(ss.str());
    } else {
        // discard aliased content
        push(expr.alias()); // replace with an alias instead
    }
}

void
DialectCompiler::operator()(const Join& join) {
    std::string condition, lhs, rhs, jointype;
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
    std::stringstream ss;

    ss << "SELECT ";
    if (select.distinct())
        ss << "DISTINCT ";
    
    size_t elements_left = select.what().size();
    BOOST_FOREACH(ExpressionPtr col, select.what()) {
        visit(*col, *this);
        ss << pop();
        --elements_left;
        if (elements_left != 0)
            ss << ", ";
    }
    
    if (select.from()) {
        in_from_clause_ = true;
        visit(*select.from(), *this);
        in_from_clause_ = false;
        ss << " FROM " << pop();
    }
    
    if (select.where()) {
        visit(*select.where(), *this);
        ss << " WHERE " << pop();
    }

    if (select.group_by().size() > 0) {
        ss << " GROUP BY ";
        elements_left = select.group_by().size();
        BOOST_FOREACH(ExpressionPtr expr, select.group_by()) {
            visit(*expr, *this);
            ss << pop();
            --elements_left;
            if (elements_left != 0)
                ss << ", ";
        }
    }
    
    if (select.order().size() > 0) {
        ss << " ORDER BY ";
        const Select::OrderVector& order = select.order();
        elements_left = order.size();
        BOOST_FOREACH(Select::OrderPair op, order) {
            visit(*op.first, *this);
            ss << pop() << " " << (op.second == Select::ASC ? "ASC" : "DESC");
            --elements_left;
            if (elements_left != 0)
                ss << ", ";
        }
    }

    if (select.limit() > 0) {
        ss << " LIMIT " << boost::lexical_cast<std::string>(select.limit());
    }

    push(ss.str());
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

    std::string stmt = "INSERT INTO " + insert.table()->name() + "(" +
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
