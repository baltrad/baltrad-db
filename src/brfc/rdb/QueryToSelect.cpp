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

#include <brfc/rdb/QueryToSelect.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/Query.hpp>
#include <brfc/StringList.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>

#include <brfc/sql/Alias.hpp>
#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Expression.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Literal.hpp>
#include <brfc/sql/Parentheses.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace rdb {

QueryToSelect::QueryToSelect(const AttributeMapper* mapper)
        : mapper_(mapper)
        , stack_()
        , model_(&Model::instance())
        , from_() {
    // always select from files and join sources
    Model* m = model_;
    from_ = m->files->join(m->sources,
                           xpr_.eq(m->files->column("source_id"),
                                   m->sources->column("id")));
}

sql::SelectPtr
QueryToSelect::transform(const Query& query,
                         const AttributeMapper& mapper) {
    QueryToSelect rpl(&mapper);

    sql::SelectPtr select = sql::Select::create();
    select->distinct(true);
    select->what(Model::instance().files->column("id"));

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), rpl);
        select->where(rpl.pop());
    } 
    
    // add the built join as from clause
    select->from(rpl.from_);

    return select;
}

void
QueryToSelect::operator()(expr::Attribute& attr) {
    String name = attr.name();

    sql::SelectablePtr value_t;

    if (name.starts_with("what/source:")) {
        // alias
        String key = name.split(":").at(1);
        if (key == "name" or key == "node") {
            push(model_->sources->column("name"));
        } else {
            String alias = "src_" + key;
            value_t = model_->source_kvs->alias(alias);

            if (not from_->contains(value_t)) {
                from_ = from_->join(value_t,
                                    xpr_.and_(xpr_.eq(value_t->column("source_id"),
                                                      model_->sources->column("id")),
                                              xpr_.eq(value_t->column("key"),
                                                      xpr_.string(key))));
            }
            push(value_t->column("value"));
        }
        return;
    }

    // query table and column where this value can be found
    Mapping mapping = mapper_->mapping(name);
    value_t = mapping.column->selectable();
    
    if (not mapper_->is_specialized(name)) {
        // try to join groups, they have to appear earlier in the join
        join_groups(); 

        // alias the table (this attribute is always searched on this table)
        String alias = name.remove("/") + "_values";
        value_t = value_t->alias(alias);

        // join this table-alias if not already joined
        if (not from_->contains(value_t)) {
            from_ = from_->join(value_t,
                                xpr_.and_(xpr_.eq(value_t->column("group_id"),
                                                  model_->groups->column("id")),
                                          xpr_.eq(value_t->column("attribute_id"),
                                                  xpr_.int64_(mapping.id))));
        }
    } else {
        if (value_t->name() == "bdb_groups") {
            join_groups();
        }
    }
    // replace the attribute with value column
    push(value_t->column(mapping.column->name()));
}

void
QueryToSelect::join_groups() {
    // join groups if not already joined
    if (not from_->contains(model_->groups)) {
        from_ = from_->join(model_->groups,
                            xpr_.eq(model_->groups->column("file_id"),
                                    model_->files->column("id")));
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

/*
void
QueryToSelect::operator()(Column& col) {
    push(col.shared_from_this());
}
*/

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
} // namespace brfc
