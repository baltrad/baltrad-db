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

#include <brfc/sql/DialectCompiler.hpp>

#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/Listcons.hpp>

#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>

using ::brfc::expr::Expression;
using ::brfc::expr::Listcons;

namespace brfc {
namespace sql {

DialectCompiler::DialectCompiler(const Dialect* dialect)
        : eval_()
        , dialect_(dialect)
        , bind_() {
    eval_.bind("alias", boost::bind(alias(), _1));
    eval_.bind("bind", boost::bind(boost::ref(bind_), _1));
    eval_.bind("+", boost::bind(binop("+"), _1));
    eval_.bind("-", boost::bind(binop("-"), _1));
    eval_.bind("*", boost::bind(binop("*"), _1));
    eval_.bind("/", boost::bind(binop("/"), _1));
    eval_.bind("!=", boost::bind(binop("!="), _1));
    eval_.bind("=", boost::bind(binop("="), _1));
    eval_.bind("<", boost::bind(binop("<"), _1));
    eval_.bind("<", boost::bind(binop("<"), _1));
    eval_.bind(">", boost::bind(binop(">"), _1));
    eval_.bind("<=", boost::bind(binop("<="), _1));
    eval_.bind(">=", boost::bind(binop(">="), _1));
    eval_.bind("and", boost::bind(binop("AND"), _1));
    eval_.bind("or", boost::bind(binop("OR"), _1));
    eval_.bind("not", boost::bind(unaryop("NOT"), _1));
    eval_.bind("like", boost::bind(like(), _1));
    eval_.bind("in", boost::bind(binop("IN"), _1));
    eval_.bind("column", boost::bind(column(), _1));
    eval_.bind("min", boost::bind(function("MIN"), _1));
    eval_.bind("max", boost::bind(function("MAX"), _1));
    eval_.bind("sum", boost::bind(function("SUM"), _1));
    eval_.bind("count", boost::bind(function("COUNT"), _1));
    eval_.bind("join", boost::bind(join("JOIN"), _1));
    eval_.bind("outerjoin", boost::bind(join("LEFT JOIN"), _1));
    eval_.bind("lit", boost::bind(literal(*dialect), _1));
    eval_.bind("label", boost::bind(label(), _1));
    eval_.bind("select", boost::bind(select(), _1));
    eval_.bind("distinct", boost::bind(distinct(), _1));
    eval_.bind("select-columns", boost::bind(select_columns(), _1));
    eval_.bind("from", boost::bind(from_clause(), _1));
    eval_.bind("where", boost::bind(where_clause(), _1));
    eval_.bind("group-by", boost::bind(group_by(), _1));
    eval_.bind("order-by", boost::bind(order_by(), _1));
    eval_.bind("asc", boost::bind(order_dir("ASC"), _1));
    eval_.bind("desc", boost::bind(order_dir("DESC"), _1));
    eval_.bind("limit", boost::bind(limit(), _1));
    eval_.bind("offset", boost::bind(offset(), _1));
    eval_.bind("insert", boost::bind(insert(), _1));
    eval_.bind("insert-columns", boost::bind(insert_columns(), _1));
    eval_.bind("insert-values", boost::bind(insert_values(), _1));
    eval_.bind("returning", boost::bind(returning(), _1));
    eval_.bind("table", boost::bind(table(), _1));
}

Query
DialectCompiler::do_compile(const Expression& expr) {
    bind_.binds_.clear();
    Expression e = eval_(expr);
    Expression q;
    if (e.is_string()) {
        q = e;
    } else {
        q = compact_str(e);
        BRFC_ASSERT(q.size() == 1);
        BRFC_ASSERT(q.front().is_string());
        q = q.front();
    }
    return Query(q.string(), bind_.binds_);
}

Expression
DialectCompiler::binop::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 2);

    Expression::const_iterator it = expr.begin();
    const Expression& lhs = *it;
    ++it;
    const Expression& rhs = *it;
    return Listcons().append(lhs).string(" " + op_ + " ").append(rhs).get();
}

Expression
DialectCompiler::like::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 2);

    Expression::const_iterator it = expr.begin();
    const Expression& lhs = *it;
    ++it;
    std::string rhs(it->string());
    boost::replace_all(rhs, "*", "%");
    boost::replace_all(rhs, "?", "_");
    return Listcons().append(lhs).string(" LIKE " + rhs).get();
}

Expression
DialectCompiler::unaryop::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 1);
    
    return Listcons().string(op_ + " ").append(expr.front()).get();
}

Expression
DialectCompiler::bind::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    std::string name = x.front().string();
    if (not boost::starts_with(name, ":"))
        name = ":" + name;
    binds_.add(name, Expression());
    return Expression(name);
//    return Listcons().symbol("bind").string(name).get();
}

Expression
DialectCompiler::column::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);
    Expression::const_iterator it = x.begin();
    const std::string& table = it->string();
    ++it;
    const std::string& column = it->string();
    return Expression(table + "." + column);
}

namespace {
/**
 * @brief join [@c first, @c last) to a string separated by @c sep
 */
std::string
join_to_str(Expression::const_iterator first,
            Expression::const_iterator last,
            const std::string& sep) {
    std::stringstream ss;
    while (first != last) {
        if (first->is_list() and first->empty()) {
            ++first;
            continue;
        }
        ss << first->string();
        ++first;
        if (first != last)
            ss << sep;
    }
    return ss.str();
}

Expression
join_with_str(Expression::const_iterator first,
              Expression::const_iterator last,
              const std::string& sep) {
    Expression e;
    while (first != last) {
        e.push_back(*first);
        ++first;
        if (first != last)
            e.push_back(Expression(sep));
    }
    return e;
}

} // namespace anonymous

Expression
DialectCompiler::function::operator()(const Expression& x) {
    Expression e;
    e.push_back(Expression(name_ + "("));
    e.extend(join_with_str(x.begin(), x.end(), ", "));
    e.push_back(Expression(")"));
    return e;
}

Expression
DialectCompiler::alias::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);

    Expression::const_iterator it = x.begin();
    const Expression& aliased = *it;
    ++it;
    const std::string& alias = it->string();

    bool is_select = false;
    if (aliased.is_list() and aliased.front().is_string()
        and ::boost::starts_with(aliased.front().string(), "SELECT ")) {
        is_select = true;
    }

    Expression e;
    if (is_select)
        e.push_back(Expression("("));
    e.push_back(aliased);
    if (is_select)
        e.push_back(Expression(")"));
    e.push_back(Expression(" AS " + alias));
    return e;
}

Expression
DialectCompiler::join::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);
    
    Expression::const_iterator it = x.begin();
    const Expression& selectable = *it;
    ++it;
    const Expression& condition = *it;

    Expression e;
    e.push_back(Expression(type_ + " "));
    e.push_back(selectable);
    if (condition) {
        e.push_back(Expression(" ON "));
        e.push_back(condition);
    }
    return e;
}

Expression
DialectCompiler::literal::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    return Expression(dialect_.literal_to_string(x.front()));
}

Expression
DialectCompiler::label::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);

    Expression::const_iterator it = x.begin();
    const Expression& lhs = *it;
    ++it;
    const std::string& rhs = it->string();
    return Listcons().append(lhs).string(" AS " + rhs).get();
}

Expression
DialectCompiler::distinct::operator()(const Expression&) {
    return Expression("DISTINCT");
}

Expression
DialectCompiler::select_columns::operator()(const Expression& x) {
    Expression e;
    e.extend(join_with_str(x.begin(), x.end(), ", "));
    return e;
}

Expression
DialectCompiler::from_clause::operator()(const Expression& x) {
    Expression e;
    e.push_back(Expression("FROM "));
    e.extend(join_with_str(x.begin(), x.end(), " "));
    return e;
}

Expression
DialectCompiler::where_clause::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);

    Expression e;
    e.push_back(Expression("WHERE "));
    e.push_back(x.front());
    return e;
}

Expression
DialectCompiler::group_by::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() >= 1);

    Expression e;
    e.push_back(Expression("GROUP BY "));
    e.extend(join_with_str(x.begin(), x.end(), ", "));
    return e;
}

Expression
DialectCompiler::order_by::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() >= 1);

    Expression e;
    e.push_back(Expression("ORDER BY "));
    e.extend(join_with_str(x.begin(), x.end(), ", "));
    return e;
}

Expression
DialectCompiler::order_dir::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    Expression e;
    e.push_back(x.front());
    e.push_back(Expression(" " + dir_));
    return e;
}

Expression
DialectCompiler::limit::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    return Expression("LIMIT " + boost::lexical_cast<std::string>(x.front().int64()));
}

Expression
DialectCompiler::offset::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    return Expression("OFFSET " + boost::lexical_cast<std::string>(x.front().int64()));
}

Expression
DialectCompiler::select::operator()(const Expression& x) {
    Expression e;
    e.push_back(Expression("SELECT "));
    e.extend(join_with_str(x.begin(), x.end(), " "));
    return e;
}

Expression
DialectCompiler::insert_columns::operator()(const Expression& x) {
    std::stringstream ss;
    ss << "(" << join_to_str(x.begin(), x.end(), ", ") << ")";
    return Expression(ss.str());
}

Expression
DialectCompiler::insert_values::operator()(const Expression& x) {
    return Listcons().string("VALUES (")
                     .extend(join_with_str(x.begin(), x.end(), ", "))
                     .string(")")
                     .get();
}

Expression
DialectCompiler::returning::operator()(const Expression& x) {
    Expression e;
    e.push_back(Expression("RETURNING "));
    e.extend(join_with_str(x.begin(), x.end(), ", "));
    return e;
}

Expression
DialectCompiler::insert::operator()(const Expression& x) {
    Expression e;
    e.push_back(Expression("INSERT INTO "));
    e.extend(join_with_str(x.begin(), x.end(), " "));
    return e;
}

Expression
DialectCompiler::table::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);

    return Expression(x.front().string());
}

Expression
DialectCompiler::compact_str(const Expression& x) {
    Expression e;
    std::string s;
    Expression::const_iterator it = x.begin();
    for ( ; it != x.end(); ++it) {
        if (it->is_string()) {
            s += it->string();
        } else if (it->is_list()) {
            const Expression& l = compact_str(*it);
            Expression::const_iterator li = l.begin();
            for ( ; li != l.end(); ++li) {
                if (li->is_string()) {
                    s+= li->string();
                } else {
                    if (not s.empty()) {
                        e.push_back(Expression(s));
                        s.clear();
                    }
                    e.push_back(*li);
                }
            }
        } else {
            if (not s.empty()) {
                e.push_back(Expression(s));
                s.clear();
            }
            e.push_back(*it);
        }
    }
    if (not s.empty())
        e.push_back(Expression(s));
    return e;
}

} // namespace sql
} // namespace brfc
