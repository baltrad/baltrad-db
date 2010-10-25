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

#include <brfc/db/rdb/FileQueryToSelect.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/StringList.hpp>

#include <brfc/db/FileQuery.hpp>
#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Parentheses.hpp>

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
namespace db {
namespace rdb {

FileQueryToSelect::FileQueryToSelect(const AttributeMapper* mapper)
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
FileQueryToSelect::transform(const FileQuery& query,
                         const AttributeMapper& mapper) {
    FileQueryToSelect rpl(&mapper);

    sql::SelectPtr select = sql::Select::create();
    select->distinct(true);
    select->what(Model::instance().files->column("id"));
    select->what(Model::instance().file_content->column("lo_id"));

    // replace attributes in where clause with columns
    if (query.filter()) {
        visit(*query.filter(), rpl);
        select->where(rpl.pop());
    } 
    
    // add the built join as from clause
    select->from(rpl.from_);

    return select;
}


sql::ColumnPtr
FileQueryToSelect::source_attr_column(expr::Attribute& attr) {
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
FileQueryToSelect::specialized_attr_column(expr::Attribute& attr) {
    Mapping mapping = mapper_->mapping(attr.name());
    // specializations are only in bdb_files, already in from clause
    return mapping.column;
}

sql::ColumnPtr
FileQueryToSelect::plain_attr_column(expr::Attribute& attr) {
    String name = attr.name();
    Mapping mapping = mapper_->mapping(name);
    
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

    // replace the attribute with value column
    return value_t->column(mapping.column->name());
}

void
FileQueryToSelect::operator()(expr::Attribute& attr) {
    String name = attr.name();
    
    sql::ColumnPtr column;
    if (name.starts_with("what/source:")) {
        column = source_attr_column(attr);
    } else if (mapper_->is_specialized(name)) {
        column = specialized_attr_column(attr);
    } else {
        column = plain_attr_column(attr);
    }

    // replace the attribute with value column
    push(column);
}

void
FileQueryToSelect::join_attrs() {
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
FileQueryToSelect::operator()(expr::BinaryOperator& op) {
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
FileQueryToSelect::operator()(Column& col) {
    push(col.shared_from_this());
}
*/

void
FileQueryToSelect::operator()(expr::Label& label) {
    visit(*label.expression(), *this);
    push(sql::Label::create(pop(), label.name()));
}

void
FileQueryToSelect::operator()(expr::Literal& literal) {
    push(sql::Literal::create(literal.value()));
}

void
FileQueryToSelect::operator()(expr::Parentheses& parentheses) {
    visit(*parentheses.expression(), *this);
    push(sql::Parentheses::create(pop()));
}

sql::ExpressionPtr
FileQueryToSelect::pop() {
    BRFC_ASSERT(!stack_.empty());
    sql::ExpressionPtr p = stack_.back();
    stack_.pop_back();
    BRFC_ASSERT(p);
    return p;
}

void
FileQueryToSelect::push(sql::ExpressionPtr p) {
    BRFC_ASSERT(p);
    stack_.push_back(p);
}

} // namespace rdb
} // namespace db
} // namespace brfc
