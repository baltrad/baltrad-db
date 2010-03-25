#include <brfc/expr/AttrReplace.hpp>

#include <brfc/assert.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Join.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Table.hpp>

#include <brfc/rdb/AttributeMapper.hpp>

#include <boost/foreach.hpp>
#include <map>
#include <algorithm>
#include <utility>

namespace brfc {
namespace expr {

AttrReplace::AttrReplace(SelectPtr select, const rdb::AttributeMapper* mapper)
        : Visitor(Visitor::POST_ORDER)
        , mapper_(mapper)
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

void
AttrReplace::replace(SelectPtr select, const rdb::AttributeMapper* mapper) {
    AttrReplace rpl(select, mapper);
    rpl.replace_attributes();
    rpl.build_from_clause();
}

void
AttrReplace::do_visit(Alias& alias) {
    alias.aliased(static_pointer_cast<Selectable>(pop()));
    push(alias.shared_from_this());
}

void
AttrReplace::do_visit(Attribute& attr) {
    QString name = attr.name();

    // query table and column where this value can be found
    rdb::Mapping mapping = mapper_->mapping(name);

    SelectablePtr value_t;

    if (not mapper_->is_specialized(name)) {
        // alias the table (this attribute is always searched on this table)
        QString alias = name.remove("/") + "_values";
        value_t = Table::create(mapping.table)->alias(alias);
        if (not from_->contains(value_t)) {
            // try to join data_objects, they have appear earlier in the join
            join_data_objects(); 
            TablePtr dobj_t = Table::create("data_objects");
            ExpressionPtr on = value_t->column("data_object_id")->eq(dobj_t->column("id"));
            on = on->and_(value_t->column("attribute_id")->eq(Literal::create(Variant(mapping.id))));
            from_ = from_->join(value_t, on);
        }
    } else {
        value_t = Table::create(mapping.table);
        if (value_t->name() == "data_objects") {
            join_data_objects();
        }
    }
    // replace the attribute with value column
    ColumnPtr col = value_t->column(mapping.column);
    col->accept(*this);
}

void
AttrReplace::join_data_objects() {
    // join data_objects if not already joined
    TablePtr files_t = Table::create("files");
    TablePtr dobj_t = Table::create("data_objects");
    if (not from_->contains(dobj_t)) {
        ExpressionPtr on = dobj_t->column("file_id")->eq(files_t->column("id"));
        from_ = from_->join(dobj_t, on);
    }
}

void
AttrReplace::do_visit(Column& column) {
    column.selectable(static_pointer_cast<Selectable>(pop()));
    push(column.shared_from_this());
}

void
AttrReplace::do_visit(BinaryOperator& op) {
    op.rhs(static_pointer_cast<Expression>(pop()));
    op.lhs(static_pointer_cast<Expression>(pop()));
    push(op.shared_from_this());
}

void
AttrReplace::do_visit(Join& join) {
    join.condition(static_pointer_cast<Expression>(pop()));
    join.to(static_pointer_cast<Selectable>(pop()));
    join.from(static_pointer_cast<Selectable>(pop()));
    push(join.shared_from_this());
}

void
AttrReplace::do_visit(Label& label) {
    label.expression(static_pointer_cast<Expression>(pop()));
    push(label.shared_from_this());
}

void
AttrReplace::do_visit(Literal& literal) {
    push(literal.shared_from_this());
}

void
AttrReplace::do_visit(Parentheses& parentheses) {
    parentheses.expression(static_pointer_cast<Expression>(pop()));
    push(parentheses.shared_from_this());
}

void
AttrReplace::do_visit(Table& table) {
    push(table.shared_from_this());
}

void
AttrReplace::do_visit(Select& select) {
    if (select.where()) {
        select.where(static_pointer_cast<Expression>(pop()));
    }

    select.from(static_pointer_cast<FromClause>(pop()));

    BOOST_FOREACH(ExpressionPtr& p, select.what()) {
        p->accept(*this);
        p = static_pointer_cast<Expression>(pop());
    }
    
    push(select.shared_from_this());
}

void
AttrReplace::do_visit(FromClause& from) {
    BOOST_REVERSE_FOREACH(SelectablePtr& p, from.elements()) {
        p = static_pointer_cast<Selectable>(pop());
    }
    push(from.shared_from_this());
}

void
AttrReplace::replace_attributes() {
    visit(*select_);
}

void
AttrReplace::build_from_clause() {
    FromClausePtr from_clause = FromClause::create();
    from_clause->add(from_);
    select_->from(from_clause);
}

ElementPtr
AttrReplace::pop() {
    BRFC_ASSERT(!stack_.empty());
    ElementPtr p = stack_.back();
    stack_.pop_back();
    BRFC_ASSERT(p);
    return p;
}

void
AttrReplace::push(ElementPtr p) {
    BRFC_ASSERT(p);
    stack_.push_back(p);
}

} // namespace expr
} // namespace brfc

