#include <brfc/expr/Compiler.hpp>

#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Visitor.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Join.hpp>
#include <brfc/expr/Label.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/Table.hpp>

#include <boost/foreach.hpp>
#include <algorithm>

#include <QtCore/QStringList>

namespace brfc {
namespace expr {

QString
Compiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    QString top = stack_.back();
    stack_.pop_back();
    return top;
}

void
Compiler::push(const QString& top) {
    stack_.push_back(top);
}


void
Compiler::do_visit(BinaryOperator& expr) {
    const QString& rhs = pop();
    const QString& lhs = pop();
    push(lhs + " " + expr.op() + " " + rhs);
}

void
Compiler::do_visit(Column& expr) {
    push(pop() + "." + expr.name());
}

void
Compiler::do_visit(Alias& expr) {
    if (in_from_clause) {
        push(pop() + " AS " + expr.alias());
    } else {
        pop(); // discard aliased content
        push(expr.alias()); // replace with an alias instead
    }
}

void
Compiler::do_visit(Attribute& expr) {
    BRFC_ASSERT(false); // attributes should be replaced at this point
    push(expr.name());
}

void
Compiler::do_visit(Join& join) {
    QString condition = pop();
    QString to = pop();
    QString from = pop();

    QString jointype;
    switch (join.type()) {
        case Join::INNER:
            jointype = " JOIN ";
            break;
        case Join::LEFT:
            jointype = " LEFT JOIN ";
            break;
        default:
            BRFC_ASSERT(false);
    }

    in_from_clause = true;
    join.to()->accept(*this);
    to = pop();
    join.from()->accept(*this);
    from = pop();
    in_from_clause = false;
    join.condition()->accept(*this);
    condition = pop();
    in_from_clause = true;

    push(from + jointype + to + " ON " + condition);
}

void
Compiler::do_visit(Literal& expr) {
    QString key = QString(":lit_") + QString::number(literal_count_++);
    push(key);
    binds_[key] = expr.value();
}

void
Compiler::do_visit(Label& label) {
    push(pop() + " AS " + label.name());
}

void
Compiler::do_visit(Parentheses& expr) {
    push("(" + pop() + ")");
}

void
Compiler::do_visit(FromClause& from) {
    if (not from.empty()) {
        QStringList from_clause_elements;
        for (size_t i = 0; i < from.elements().size(); ++i) {
            from_clause_elements.push_back(pop());
        }
        std::reverse(from_clause_elements.begin(), from_clause_elements.end());
        QString from_clause = "\nFROM " + from_clause_elements.join(", ");
        push(from_clause);
    }
}

void
Compiler::do_visit(Select& select) {
    QString where_clause;
    if (select.where())
        where_clause = "\nWHERE " + pop();
    QString from_clause;
    if (!select.from()->empty())
        from_clause = pop();
    
    QStringList result_column_elm;
    for (size_t i = 0; i < select.what().size(); ++i) {
        result_column_elm.push_back(pop());
    }
    std::reverse(result_column_elm.begin(), result_column_elm.end());
    QString result_columns = result_column_elm.join(", ");

    QString distinct = select.distinct() ? "DISTINCT " : "";
    // SELECT columns FROM from_obj WHERE where_clause
    QString clause = "SELECT " + distinct
                               + result_columns
                               + from_clause
                               + where_clause;
    push(clause);
}

void
Compiler::do_visit(Table& expr) {
    push(expr.name());
}

} // namespace expr
} // namespace brfc
