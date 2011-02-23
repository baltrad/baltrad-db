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

#include <brfc/db/rdb/QueryToSelect.hpp>

#include <list>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/assert.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/FileQuery.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionList.hpp>
#include <brfc/expr/Function.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Expression.hpp>
#include <brfc/sql/ExpressionList.hpp>
#include <brfc/sql/Function.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace db {
namespace rdb {

QueryToSelect::QueryToSelect(const AttributeMapper* mapper)
        : mapper_(mapper)
        , stack_()
        , m_(Model::instance())
        , select_()
        , from_() {
}

void
QueryToSelect::reset() {
    select_ = sql::Select::create();
    // always select from files and join sources
    from_ = m_.files->join(m_.sources,
                           xpr_.eq(m_.files->column("source_id"),
                                   m_.sources->column("id")));
    stack_.clear();
}

sql::SelectPtr
QueryToSelect::transform(const FileQuery& query) {
    reset();

    select_->what(m_.files->column("id"));
    select_->limit(query.limit());
    select_->offset(query.skip());

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), *this);
        select_->where(pop());
    } 

    if (query.order().size() > 0 ) {
        BOOST_FOREACH(FileQuery::OrderPair op, query.order()) {
            visit(*op.first, *this);
            sql::ExpressionPtr order = pop();
            if (op.second == FileQuery::ASC) {
                order = sql::Function::min(order);
            } else {
                order = sql::Function::max(order);
            }
            select_->append_order_by(
                order,
                sql::Select::SortDirection(op.second)
            );
        }
        select_->append_group_by(m_.files->column("id"));
    } else {
        select_->append_order_by(m_.files->column("id"), sql::Select::ASC);
        select_->distinct(true);
    }
    
    // add the built join as from clause
    select_->from(from_);

    return select_;
}

sql::SelectPtr
QueryToSelect::transform(const AttributeQuery& query) {
    reset();
    
    select_->distinct(query.distinct());
    select_->limit(query.limit());

    // replace attributes with columns
    BOOST_FOREACH(const AttributeQuery::FetchMap::value_type& val, query.fetch()) {
        visit(*val.second, *this);
        select_->what(pop()->label("l_" + val.first));
    }

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), *this);
        select_->where(pop());
    } 

    // replace attributes in group by
    BOOST_FOREACH(expr::ExpressionPtr expr, query.group()) {
        visit(*expr, *this);
        select_->append_group_by(pop());
    }

    // replace attributes in order by
    BOOST_FOREACH(AttributeQuery::OrderPair op, query.order()) {
        visit(*op.first, *this);
        select_->append_order_by(pop(), sql::Select::SortDirection(op.second));
    }
    
    // add the built join as from clause
    select_->from(from_);

    return select_;
}

sql::ColumnPtr
QueryToSelect::source_attr_column(const expr::Attribute& attr) {
    const std::string& attr_name = attr.name();
    const std::string& key = attr_name.substr(attr_name.find_first_of(':') + 1);
    if (key == "_name") {
        // sources is joined by default
        // this attribute can be accessed at sources.name
        return m_.sources->column("name");
    } else {
        std::string alias = "src_" + key;
        sql::AliasPtr value_t = m_.source_kvs->alias(alias);

        // join if missing
        if (not from_->contains(value_t)) {
            from_ = from_->outerjoin(value_t,
                        xpr_.and_(
                            xpr_.eq(
                                value_t->column("source_id"),
                                m_.sources->column("id")),
                            xpr_.eq(
                                value_t->column("key"),
                                xpr_.string(key))
                        )
            );
        }

        // this attribute can be accessed at src_$KEY.value
        return value_t->column("value");
    }
}

sql::ColumnPtr
QueryToSelect::specialized_attr_column(const expr::Attribute& attr) {
    // currently, specialized attributes are only in bdb_files and
    // bdb_sources. These are already contained in the from clause.
    Mapping mapping = mapper_->mapping(attr.name());
    return mapping.column;
}

sql::ColumnPtr
QueryToSelect::plain_attr_column(const expr::Attribute& attr) {
    std::string name = attr.name();
    
    std::list<std::string> path;
    boost::split(path, name, boost::is_any_of("/"), boost::token_compress_on);
    std::string attrname = path.back();
    path.pop_back();
    std::string groupname;
    if (not path.empty()) {
        groupname = path.back();
        path.pop_back();
    }
    
    // alias the table (this attribute is always searched on this alias)
    boost::erase_all(name, "/");
    sql::AliasPtr value_t = m_.attrvals->alias(name + "_values");
    
    // join this table-alias if not already joined
    if (not from_->contains(value_t)) {
        // join attribute layer to files
        sql::SelectablePtr l0 = m_.nodes->alias(name + "_l0");
        from_ = from_->outerjoin(l0,
            xpr_.and_(
                l0->column("file_id")->eq(m_.files->column("id")),
                l0->column("name")->eq(xpr_.string(attrname))
            )
        );
        // join group layer to attribute layer
        sql::SelectablePtr l1 = m_.nodes->alias(name + "_l1");
        from_ = from_->outerjoin(l1,
            xpr_.and_(
                l1->column("id")->eq(l0->column("parent_id")),
                l1->column("name")->eq(xpr_.string(groupname))
            )
        );
        // join attribute values to attribute layer
        from_ = from_->outerjoin(value_t,
            value_t->column("node_id")->eq(l0->column("id"))
        );
    }

    switch (attr.type()) {
        case expr::Attribute::STRING:
            return value_t->column("value_str");
        case expr::Attribute::INT64:
            return value_t->column("value_int");
        case expr::Attribute::DOUBLE:
            return value_t->column("value_double");
        case expr::Attribute::BOOL:
            return value_t->column("value_bool");
        case expr::Attribute::DATE:
            return value_t->column("value_date");
        case expr::Attribute::TIME:
            return value_t->column("value_time");
        default:
            throw std::runtime_error("unhandled attribute type");
    }
}

void
QueryToSelect::operator()(const expr::Attribute& attr) {
    const std::string& name = attr.name();
    
    sql::ColumnPtr column;
    if (boost::starts_with(name, "what/source:")) {
        column = source_attr_column(attr);
    } else if (mapper_->has(name)) {
        column = specialized_attr_column(attr);
    } else {
        column = plain_attr_column(attr);
    }

    // replace the attribute with value column
    push(column);
}

namespace {

sql::ExpressionPtr
replace_pattern(sql::ExpressionPtr expr) {
    sql::LiteralPtr l = dynamic_pointer_cast<sql::Literal>(expr);
    if (l and l->value().is_string()) {
        std::string value(l->value().string());
        boost::replace_all(value, "*", "%");
        boost::replace_all(value, "?", "_");
        l = sql::Literal::create(Variant(value));
        return l;
    }
    return expr;
}

std::string
sql_operator(expr::BinaryOperator::Op op) {
    switch (op) {
        case expr::BinaryOperator::NE: return "!=";
        case expr::BinaryOperator::EQ: return "=";
        case expr::BinaryOperator::GT: return ">";
        case expr::BinaryOperator::LT: return "<";
        case expr::BinaryOperator::LE: return "<=";
        case expr::BinaryOperator::GE: return ">=";
        case expr::BinaryOperator::LIKE: return "LIKE";
        case expr::BinaryOperator::IN: return "IN";
        case expr::BinaryOperator::NOT_IN: return "NOT IN";
        case expr::BinaryOperator::AND: return "AND";
        case expr::BinaryOperator::OR: return "OR";
        case expr::BinaryOperator::ADD: return "+";
        case expr::BinaryOperator::SUB: return "-";
        case expr::BinaryOperator::MUL: return "*";
        case expr::BinaryOperator::DIV: return "/";
    }
    throw std::runtime_error("");
}
 
} // namespace anonymous

void
QueryToSelect::operator()(const expr::BinaryOperator& op) {
    visit(*op.lhs(), *this);
    visit(*op.rhs(), *this);
    sql::ExpressionPtr rhs = pop();
    sql::ExpressionPtr lhs = pop();
    if (op.op() == expr::BinaryOperator::LIKE) {
        rhs = replace_pattern(rhs);
    }
    push(sql::BinaryOperator::create(sql_operator(op.op()), lhs, rhs));
}

void
QueryToSelect::operator()(const expr::ExpressionList& exprs) {
    sql::ExpressionListPtr sql_exprs = sql::ExpressionList::create();
    for (size_t i = 0; i < exprs.size(); ++i) {
        visit(exprs.at(i), *this);
        sql_exprs->add(pop());
    }

    push(sql_exprs->parentheses());
}

void
QueryToSelect::operator()(const expr::Function& func) {
    sql::FunctionPtr f = sql::Function::create(func.name());

    BOOST_FOREACH(expr::ExpressionPtr arg, func.args()) {
        visit(*arg, *this);
        f->add_arg(pop());
    }

    push(f);
}

void
QueryToSelect::operator()(const expr::Literal& literal) {
    push(sql::Literal::create(literal.value()));
}

void
QueryToSelect::operator()(const expr::Parentheses& parentheses) {
    visit(parentheses.expression(), *this);
    push(sql::Parentheses::create(pop()));
}

sql::ExpressionPtr
QueryToSelect::pop() {
    BRFC_ASSERT(!stack_.empty());
    sql::ExpressionPtr p = stack_.back();
    stack_.pop_back();
    BRFC_ASSERT(p);
    return p;
}

void
QueryToSelect::push(sql::ExpressionPtr p) {
    BRFC_ASSERT(p);
    stack_.push_back(p);
}

} // namespace rdb
} // namespace db
} // namespace brfc
