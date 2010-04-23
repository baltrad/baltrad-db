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

QueryToSelect::QueryToSelect(const AttributeMapper* mapper)
        : mapper_(mapper)
        , stack_()
        , files_t_(Table::create("bdb_files"))
        , src_t_(Table::create("bdb_sources"))
        , src_radars_t_(Table::create("bdb_source_radars"))
        , src_centres_t_(Table::create("bdb_source_centres"))
        , groups_t_(Table::create("bdb_groups"))
        , from_() {
    // always select from files and join sources
    from_ = files_t_->join(src_t_,
                           xpr_.eq(files_t_->column("source_id"),
                                   src_t_->column("id")));
    from_ = from_->outerjoin(src_radars_t_,
                             xpr_.eq(src_t_->column("id"),
                                     src_radars_t_->column("id")));
    from_ = from_->outerjoin(src_centres_t_,
                             xpr_.eq(src_t_->column("id"),
                                     src_centres_t_->column("id")));
}

SelectPtr
QueryToSelect::transform(const Query& query,
                         const AttributeMapper& mapper) {
    QueryToSelect rpl(&mapper);

    SelectPtr select = Select::create();
    select->distinct(query.distinct());

    // replace attributes with columns
    BOOST_FOREACH(expr::AttributePtr expr, query.fetch()) {
        visit(*expr, rpl);
        select->what(rpl.pop());
    }

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), rpl);
        select->where(rpl.pop());
    } 
    
    // add the built join as from clause
    select->from()->add(rpl.from_);

    return select;
}

void
QueryToSelect::operator()(expr::Attribute& attr) {
    QString name = attr.name();

    // query table and column where this value can be found
    Mapping mapping = mapper_->mapping(name);
    SelectablePtr value_t = Table::create(mapping.table);
    
    if (not mapper_->is_specialized(name)) {
        // try to join groups, they have to appear earlier in the join
        join_groups(); 

        // alias the table (this attribute is always searched on this table)
        QString alias = name.remove("/") + "_values";
        value_t = value_t->alias(alias);

        // join this table-alias if not already joined
        if (not from_->contains(value_t)) {
            from_ = from_->join(value_t,
                                xpr_.and_(xpr_.eq(value_t->column("group_id"),
                                                  groups_t_->column("id")),
                                          xpr_.eq(value_t->column("attribute_id"),
                                                  xpr_.integer(mapping.id))));
        }
    } else {
        if (value_t->name() == "bdb_groups") {
            join_groups();
        }
    }
    // replace the attribute with value column
    push(value_t->column(mapping.column));
}

void
QueryToSelect::join_groups() {
    // join groups if not already joined
    if (not from_->contains(groups_t_)) {
        from_ = from_->join(groups_t_,
                            xpr_.eq(groups_t_->column("file_id"),
                                    files_t_->column("id")));
    }
}

void
QueryToSelect::operator()(expr::BinaryOperator& op) {
    visit(*op.lhs(), *this);
    visit(*op.rhs(), *this);
    op.rhs(pop());
    op.lhs(pop());
    push(op.shared_from_this());
}

void
QueryToSelect::operator()(expr::Label& label) {
    visit(*label.expression(), *this);
    label.expression(pop());
    push(label.shared_from_this());
}

void
QueryToSelect::operator()(expr::Literal& literal) {
    push(literal.shared_from_this());
}

void
QueryToSelect::operator()(expr::Parentheses& parentheses) {
    visit(*parentheses.expression(), *this);
    parentheses.expression(pop());
    push(parentheses.shared_from_this());
}

expr::ExpressionPtr
QueryToSelect::pop() {
    BRFC_ASSERT(!stack_.empty());
    expr::ExpressionPtr p = stack_.back();
    stack_.pop_back();
    BRFC_ASSERT(p);
    return p;
}

void
QueryToSelect::push(expr::ExpressionPtr p) {
    BRFC_ASSERT(p);
    stack_.push_back(p);
}

} // namespace rdb
} // namespace brfc
