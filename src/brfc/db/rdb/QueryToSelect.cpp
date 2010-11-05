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

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/StringList.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/FileQuery.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Function.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Expression.hpp>
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
        , model_(&Model::instance())
        , attrs_(model_->nodes->alias("attrs"))
        , pnode1_(model_->nodes->alias("pnode1"))
        , from_() {
    // always select from files and join sources
    Model* m = model_;
    from_ = m->files->join(m->sources,
                           xpr_.eq(m->files->column("source_id"),
                                   m->sources->column("id")));
    from_ = from_->join(m->file_content,
                        xpr_.eq(m->file_content->column("file_id"),
                                m->files->column("id")));
}

sql::SelectPtr
QueryToSelect::transform(const FileQuery& query,
                         const AttributeMapper& mapper) {
    QueryToSelect rpl(&mapper);
    const Model& m = Model::instance();

    sql::SelectPtr select = sql::Select::create();
    select->distinct(true);
    select->what(m.files->column("id"));
    select->what(m.file_content->column("lo_id"));
    select->append_order_by(m.files->column("id"), sql::Select::ASC);

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), rpl);
        select->where(rpl.pop());
    } 
    
    // add the built join as from clause
    select->from(rpl.from_);

    return select;
}

sql::SelectPtr
QueryToSelect::transform(const AttributeQuery& query,
                         const AttributeMapper& mapper) {
    QueryToSelect rpl(&mapper);

    sql::SelectPtr select = sql::Select::create();
    select->distinct(query.distinct());
    select->limit(query.limit());

    // replace attributes with columns
    BOOST_FOREACH(expr::ExpressionPtr expr, query.fetch()) {
        visit(*expr, rpl);
        select->what(rpl.pop());
    }

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), rpl);
        select->where(rpl.pop());
    } 

    // replace attributes in order by
    BOOST_FOREACH(AttributeQuery::OrderPair op, query.order()) {
        visit(*op.first, rpl);
        select->append_order_by(rpl.pop(), sql::Select::SortDirection(op.second));
    }
    
    // add the built join as from clause
    select->from(rpl.from_);

    return select;
}

sql::ColumnPtr
QueryToSelect::source_attr_column(expr::Attribute& attr) {
    String key = attr.name().split(":").at(1);
    if (key == "name" or key == "node") {
        // sources is joined by default
        // this attribute can be accessed at sources.name
        return model_->sources->column("name");
    } else {
        String alias = "src_" + key;
        sql::AliasPtr value_t = model_->source_kvs->alias(alias);

        // join if missing
        if (not from_->contains(value_t)) {
            from_ = from_->outerjoin(value_t,
                        xpr_.and_(
                            xpr_.eq(
                                value_t->column("source_id"),
                                model_->sources->column("id")),
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
QueryToSelect::specialized_attr_column(expr::Attribute& attr) {
    // currently, specialized attributes are only in bdb_files and
    // bdb_sources. These are already contained in the from clause.
    Mapping mapping = mapper_->mapping(attr.name());
    return mapping.column;
}

sql::ColumnPtr
QueryToSelect::plain_attr_column(expr::Attribute& attr) {
    String name = attr.name();
    
    StringList path = name.split("/");
    String attrname = path.take_last();
    String groupname;
    if (not path.empty())
        groupname = path.take_last();

    join_attrs(); 

    // alias the table (this attribute is always searched on this alias)
    String alias = name.remove("/") + "_values";
    sql::AliasPtr value_t = model_->attrvals->alias(alias);
    
    // join this table-alias if not already joined
    if (not from_->contains(value_t)) {
        from_ = from_->outerjoin(value_t,
                    value_t->column("node_id")->eq(attrs_->column("id"))
                    ->and_(attrs_->column("name")->eq(xpr_.string(attrname)))
                    ->and_(pnode1_->column("name")->eq(xpr_.string(groupname)))->parentheses()
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
QueryToSelect::operator()(expr::Attribute& attr) {
    String name = attr.name();
    
    sql::ColumnPtr column;
    if (name.starts_with("what/source:")) {
        column = source_attr_column(attr);
    } else if (mapper_->has(name)) {
        column = specialized_attr_column(attr);
    } else {
        column = plain_attr_column(attr);
    }

    // replace the attribute with value column
    push(column);
}

void
QueryToSelect::join_attrs() {
    // join attrs if not already joined
    if (not from_->contains(attrs_)) {
        from_ = from_->join(attrs_,
                            xpr_.eq(attrs_->column("file_id"),
                                    model_->files->column("id")));
        from_ = from_->join(pnode1_,
                            xpr_.eq(pnode1_->column("id"),
                                    attrs_->column("parent_id")));
    }
}

namespace {

sql::ExpressionPtr
replace_pattern(sql::ExpressionPtr expr) {
    sql::LiteralPtr l = dynamic_pointer_cast<sql::Literal>(expr);
    if (l and l->value().is_string()) {
        String value(l->value().string());
        value.replace("*", "%");
        value.replace("?", "_");
        l = sql::Literal::create(Variant(value));
        return l;
    }
    return expr;
}
 
} // namespace anonymous

void
QueryToSelect::operator()(expr::BinaryOperator& op) {
    visit(*op.lhs(), *this);
    visit(*op.rhs(), *this);
    sql::ExpressionPtr rhs = pop();
    sql::ExpressionPtr lhs = pop();
    if (op.op() == "LIKE") {
        rhs = replace_pattern(rhs);
    }
    push(sql::BinaryOperator::create(op.op(), lhs, rhs));
}

void
QueryToSelect::operator()(expr::Function& func) {
    sql::FunctionPtr f = sql::Function::create(func.name());

    BOOST_FOREACH(expr::ExpressionPtr arg, func.args()) {
        visit(*arg, *this);
        f->add_arg(pop());
    }

    push(f);
}

void
QueryToSelect::operator()(expr::Label& label) {
    visit(*label.expression(), *this);
    push(sql::Label::create(pop(), label.name()));
}

void
QueryToSelect::operator()(expr::Literal& literal) {
    push(sql::Literal::create(literal.value()));
}

void
QueryToSelect::operator()(expr::Parentheses& parentheses) {
    visit(*parentheses.expression(), *this);
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
