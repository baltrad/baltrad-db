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
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <brfc/assert.hpp>
#include <brfc/expr/Expression.hpp>

#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Query.hpp>
#include <brfc/sql/Select.hpp>

using ::brfc::expr::Expression;

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
    Expression e;
    try {
        e = eval_(expr);
    } catch (...) {
        std::cout << expr << std::endl;
        throw;
    }
    BRFC_ASSERT(e.is_string());
    return Query(e.string(), bind_.binds_);
}

Expression
DialectCompiler::binop::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 2);

    Expression::const_iterator it = expr.begin();
    std::string lhs(it->string());
    ++it;
    std::string rhs(it->string());
    return Expression(lhs + " " + op_ + " " + rhs);
}

Expression
DialectCompiler::like::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 2);

    Expression::const_iterator it = expr.begin();
    std::string lhs(it->string());
    ++it;
    std::string rhs(it->string());
    boost::replace_all(rhs, "*", "%");
    boost::replace_all(rhs, "?", "_");
    return Expression(lhs + " LIKE " + rhs);
}

Expression
DialectCompiler::unaryop::operator()(const Expression& expr) {
    BRFC_ASSERT(expr.size() == 1);

    return Expression(op_ + " " + expr.front().string());
}

Expression
DialectCompiler::bind::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    std::string name = x.front().string();
    if (not boost::starts_with(name, ":"))
        name = ":" + name;
    binds_.add(name, Variant());
    return Expression(name);
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
DialectCompiler::function::operator()(const Expression& x) {
    std::stringstream ss;
    ss << name_ << "(" << join_to_str(x.begin(), x.end(), ", ") << ")";
    return Expression(ss.str());
}

Expression
DialectCompiler::alias::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);

    Expression::const_iterator it = x.begin();
    const std::string& aliased = it->string();
    ++it;
    const std::string& alias = it->string();

    std::stringstream ss;

    bool is_select = ::boost::starts_with(aliased, "SELECT ");
    if (is_select) ss << "(";
    ss << aliased;
    if (is_select) ss << ")";
    ss << " AS " << alias;
    return Expression(ss.str());
}

Expression
DialectCompiler::join::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);

    std::stringstream ss;
    ss << type_ << " ";

    Expression::const_iterator it = x.begin();
    const std::string& table = it->string();
    ss << table;
    
    ++it;
    const std::string& condition = it->string();
    if (not condition.empty())
        ss << " ON " << condition;
    return Expression(ss.str());
}

std::string
literal_to_string(const Expression& x) {
    switch (x.type()) {
        case Expression::type::BOOL:
            return x.bool_() ? "true" : "false";
        case Expression::type::INT64:
            return boost::lexical_cast<std::string>(x.int64());
        case Expression::type::DOUBLE:
            return boost::lexical_cast<std::string>(x.double_());
        case Expression::type::STRING:
            return "'" + x.string() + "'";
        case Expression::type::DATE:
            return "'" + x.date().to_iso_string(true) + "'";
        case Expression::type::TIME:
            return "'" + x.time().to_iso_string(true) + "'";
        case Expression::type::DATETIME:
            return "'" + x.datetime().to_iso_string(true) + "'";
        default:
            throw std::logic_error("invalid literal");
    }
}

Expression
DialectCompiler::literal::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    if (x.front().is_list()) {
        if (x.front().empty())
            return Expression("NULL");
        std::vector<std::string> strs;
        BOOST_FOREACH(const Expression& e, x.front()) {
            strs.push_back(literal_to_string(e));
        }
        return Expression("(" + boost::join(strs, ", ") + ")");
    }
    return Expression(literal_to_string(x.front()));
}

Expression
DialectCompiler::label::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 2);

    std::stringstream ss;
    Expression::const_iterator it = x.begin();
    ss << it->string() << " AS ";
    ++it;
    ss << it->string();
    return Expression(ss.str());
}

Expression
DialectCompiler::distinct::operator()(const Expression&) {
    return Expression("DISTINCT");
}

Expression
DialectCompiler::select_columns::operator()(const Expression& x) {
    return Expression(join_to_str(x.begin(), x.end(), ", "));
}

Expression
DialectCompiler::from_clause::operator()(const Expression& x) {
    std::stringstream ss;
    Expression::const_iterator it = x.begin();
    ss << "FROM " << join_to_str(it, x.end(), " ");
    return Expression(ss.str());
}

Expression
DialectCompiler::where_clause::operator()(const Expression& x) {
    if (not x.empty()) {
        std::stringstream ss;
        ss << "WHERE " << x.front().string();
        return Expression(ss.str());
    } else {
        return x;
    }
}

Expression
DialectCompiler::group_by::operator()(const Expression& x) {
    if (not x.empty()) {
        std::stringstream ss;
        ss << "GROUP BY " << join_to_str(x.begin(), x.end(), ", ");
        return Expression(ss.str());
    } else {
        return x;
    }
}

Expression
DialectCompiler::order_by::operator()(const Expression& x) {
    if (not x.empty()) {
        std::stringstream ss;
        ss << "ORDER BY " << join_to_str(x.begin(), x.end(), ", ");
        return Expression(ss.str());
    } else {
        return x;
    }
}

Expression
DialectCompiler::order_dir::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);
    
    std::stringstream ss;
    ss << x.front().string() << " " << dir_;
    return Expression(ss.str());
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
    return Expression("SELECT " + join_to_str(x.begin(), x.end(), " "));
}

Expression
DialectCompiler::insert_columns::operator()(const Expression& x) {
    std::stringstream ss;
    ss << "(" << join_to_str(x.begin(), x.end(), ", ") << ")";
    return Expression(ss.str());
}

Expression
DialectCompiler::insert_values::operator()(const Expression& x) {
    std::stringstream ss;
    ss << "VALUES (" << join_to_str(x.begin(), x.end(), ", ") << ")";
    return Expression(ss.str());
}

Expression
DialectCompiler::returning::operator()(const Expression& x) {
    std::stringstream ss;
    ss << "RETURNING " << join_to_str(x.begin(), x.end(), ", ");
    return Expression(ss.str());
}

Expression
DialectCompiler::insert::operator()(const Expression& x) {
    std::stringstream ss;
    ss << "INSERT INTO " << join_to_str(x.begin(), x.end(), " ");
    return Expression(ss.str());
}

Expression
DialectCompiler::table::operator()(const Expression& x) {
    BRFC_ASSERT(x.size() == 1);

    return Expression(x.front().string());
}

} // namespace sql
} // namespace brfc
