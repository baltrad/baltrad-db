#include <brfc/Query.hpp>

#include <brfc/assert.hpp>
#include <brfc/Database.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Visitor.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Table.hpp>
#include <brfc/expr/Compiler.hpp>

#include <boost/foreach.hpp>
#include <map>

namespace brfc {

Query::Query(Database* db, const AttributeMapper* mapper)
        : db_(db)
        , mapper_(mapper)
        , select_(expr::Select::create()) {
}

Query::Query(const Query& other)
        : db_(other.db_)
        , select_(other.select_) {
}

Query::~Query() {
    
}

namespace {

using namespace brfc::expr;

class AttrReplace : public Visitor {
  public:
    AttrReplace(SelectPtr select, const AttributeMapper* mapper)
            : Visitor(Visitor::POST_ORDER)
            , mapper_(mapper)
            , stack_()
            , select_(select)
            , join_where_() {
    }

    virtual void do_visit(brfc::expr::Alias& alias) {
        alias.aliased(static_pointer_cast<Selectable>(pop()));
        push(alias.shared_from_this());
    }

    /**
     * replace this attribute with a column
     * if it's not a specialized column,
     * add a where clause to attribute names
     */
    virtual void do_visit(brfc::expr::Attribute& attr) {
        const std::string& name = attr.name();

        // query table and column where this value can be found
        Mapping tc = mapper_->mapping(name);

        SelectablePtr value_t;

        // if not specialized
        if (!mapper_->is_specialized(name)) {
            // alias the table (this attribute is always searched on this table)
            value_t = Table::create(tc.table)->alias(name + "_values");
            TablePtr data_objects_t = Table::create("data_objects");
            TablePtr files_t = Table::create("files");
            // if not already defined, join
            if (!select_->from()->has(value_t)) {
                select_->from()->add(value_t);
                int id = mapper_->spec(attr.name()).id;
                ExpressionPtr expr = value_t->column("attribute_id")->eq(Literal::create(id));
                join_where(expr);
                join_where(value_t->column("data_object_id")->eq(data_objects_t->column("id")));
            }
            if (!select_->from()->has(data_objects_t)) {
                select_->from()->add(data_objects_t);
                join_where(data_objects_t->column("file_id")->eq(files_t->column("id")));
            }
            
        } else {
            value_t = Table::create(tc.table);
            if (!select_->from()->has(value_t))
                select_->from()->add(value_t);
        }
        // replace the attribute with value column
        ColumnPtr col = value_t->column(tc.column);
        col->accept(*this);
    }

    virtual void do_visit(Column& column) {
        column.selectable(static_pointer_cast<Selectable>(pop()));
        push(column.shared_from_this());
    }

    virtual void do_visit(BinaryOperator& op) {
        op.rhs(static_pointer_cast<Expression>(pop()));
        op.lhs(static_pointer_cast<Expression>(pop()));
        push(op.shared_from_this());
    }

    virtual void do_visit(Label& label) {
        label.expression(static_pointer_cast<Expression>(pop()));
        push(label.shared_from_this());
    }

    virtual void do_visit(Literal& literal) {
        push(literal.shared_from_this());
    }

    virtual void do_visit(Parentheses& parentheses) {
        parentheses.expression(static_pointer_cast<Expression>(pop()));
        push(parentheses.shared_from_this());
    }

    virtual void do_visit(Table& table) {
        push(table.shared_from_this());
    }

    virtual void do_visit(Select& select) {
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

    virtual void do_visit(FromClause& from) {
        BOOST_REVERSE_FOREACH(SelectablePtr& p, from.elements()) {
            p = static_pointer_cast<Selectable>(pop());
        }
        push(from.shared_from_this());
    }

    void join_where(ExpressionPtr where) {
        join_where_ = join_where_ ? join_where_->and_(where) : where;
    }

    void join() {
        if (select_->where()) {
            select_->where(select_->where()->parentheses());
        }
        if (join_where_) {
            select_->append_where(join_where_->parentheses());
        }
    }

    ElementPtr pop() {
        BRFC_ASSERT(!stack_.empty());
        ElementPtr p = stack_.back();
        stack_.pop_back();
        BRFC_ASSERT(p);
        return p;
    }

    void push(ElementPtr p) {
        BRFC_ASSERT(p);
        stack_.push_back(p);
    }

  private:
    const AttributeMapper* mapper_;
    std::vector<ElementPtr> stack_;
    SelectPtr select_;
    ExpressionPtr join_where_;
};

} // namespace anonymous

Query&
Query::distinct(bool distinct) {
    select_->distinct(distinct);
    return *this;
}

Query&
Query::fetch(expr::AttributePtr attribute) {
    select_->what(attribute);
    return *this;
}

Query&
Query::filter(expr::ExpressionPtr expr) {
    // replace attribute
    select_->append_where(expr);
    return *this;
}

ResultSet
Query::execute() {
    expr::TablePtr data_objects_t = expr::Table::create("data_objects");
    expr::TablePtr files_t = expr::Table::create("files");
    select_->from()->add(files_t);

    AttrReplace r(select_, mapper_);
    r.visit(*select_);
    r.join();
    expr::Compiler compiler;
    compiler.compile(*select_);

    return db_->query(QString::fromStdString(compiler.compiled()), compiler.binds());
}

} // namespace brfc
