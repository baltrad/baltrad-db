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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/rdb/QueryToSelect.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/Query.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Literal.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Alias.hpp>
#include <brfc/rdb/Column.hpp>
#include <brfc/rdb/FromClause.hpp>
#include <brfc/rdb/Join.hpp>
#include <brfc/rdb/Select.hpp>
#include <brfc/rdb/Table.hpp>

namespace brfc {
namespace rdb {

using namespace ::brfc::expr;

QueryToSelect::QueryToSelect(SelectPtr select, const AttributeMapper* mapper)
        : mapper_(mapper)
        , stack_()
        , select_(select)
        , from_() {
    // always select from files and sources
    TablePtr files_t = Table::create("files");
    TablePtr src_t = Table::create("sources");
    TablePtr src_radars_t = Table::create("source_radars");
    TablePtr src_centres_t = Table::create("source_centres");
    
    ExpressionPtr on = files_t->column("source_id")->eq(src_t->column("id"));
    from_ = files_t->join(src_t, on);
    on = src_t->column("id")->eq(src_radars_t->column("id"));
    from_ = from_->outerjoin(src_radars_t, on);
    on = src_t->column("id")->eq(src_centres_t->column("id"));
    from_ = from_->outerjoin(src_centres_t, on);
}

SelectPtr
QueryToSelect::transform(const Query& query,
                         const AttributeMapper& mapper) {
    SelectPtr select = Select::create();

    BOOST_FOREACH(expr::AttributePtr expr, query.fetch()) {
        select->what(expr);
    }

    select->where(query.filter());
    select->distinct(query.distinct());


    QueryToSelect rpl(select, &mapper);
    rpl.replace_attributes();
    rpl.build_from_clause();
    return select;
}

void
QueryToSelect::operator()(Attribute& attr) {
    QString name = attr.name();

    // query table and column where this value can be found
    Mapping mapping = mapper_->mapping(name);

    SelectablePtr value_t;

    if (not mapper_->is_specialized(name)) {
        // alias the table (this attribute is always searched on this table)
        QString alias = name.remove("/") + "_values";
        value_t = Table::create(mapping.table)->alias(alias);
        if (not from_->contains(value_t)) {
            // try to join groups, they have to appear earlier in the join
            join_groups(); 
            TablePtr grp_t = Table::create("groups");
            ExpressionPtr on = value_t->column("group_id")->eq(grp_t->column("id"));
            on = on->and_(value_t->column("attribute_id")->eq(Literal::create(Variant(mapping.id))));
            from_ = from_->join(value_t, on);
        }
    } else {
        value_t = Table::create(mapping.table);
        if (value_t->name() == "groups") {
            join_groups();
        }
    }
    // replace the attribute with value column
    ColumnPtr col = value_t->column(mapping.column);
    push(col);
}

void
QueryToSelect::join_groups() {
    // join groups if not already joined
    TablePtr files_t = Table::create("files");
    TablePtr grp_t = Table::create("groups");
    if (not from_->contains(grp_t)) {
        ExpressionPtr on = grp_t->column("file_id")->eq(files_t->column("id"));
        from_ = from_->join(grp_t, on);
    }
}

void
QueryToSelect::operator()(BinaryOperator& op) {
    visit(*op.lhs(), *this);
    visit(*op.rhs(), *this);
    op.rhs(static_pointer_cast<Expression>(pop()));
    op.lhs(static_pointer_cast<Expression>(pop()));
    push(op.shared_from_this());
}

void
QueryToSelect::operator()(Label& label) {
    visit(*label.expression(), *this);
    label.expression(static_pointer_cast<Expression>(pop()));
    push(label.shared_from_this());
}

void
QueryToSelect::operator()(Literal& literal) {
    push(literal.shared_from_this());
}

void
QueryToSelect::operator()(Parentheses& parentheses) {
    visit(*parentheses.expression(), *this);
    parentheses.expression(static_pointer_cast<Expression>(pop()));
    push(parentheses.shared_from_this());
}

void
QueryToSelect::replace_attributes() {
    if (select_->where()) {
        visit(*select_->where(), *this);
        select_->where(static_pointer_cast<Expression>(pop()));
    } 

    BOOST_FOREACH(ExpressionPtr& p, select_->what()) {
        visit(*p, *this);
        p = static_pointer_cast<Expression>(pop());
    }
}

void
QueryToSelect::build_from_clause() {
    FromClausePtr from_clause = FromClause::create();
    from_clause->add(from_);
    select_->from(from_clause);
}

ElementPtr
QueryToSelect::pop() {
    BRFC_ASSERT(!stack_.empty());
    ElementPtr p = stack_.back();
    stack_.pop_back();
    BRFC_ASSERT(p);
    return p;
}

void
QueryToSelect::push(ElementPtr p) {
    BRFC_ASSERT(p);
    stack_.push_back(p);
}

} // namespace rdb
} // namespace brfc

