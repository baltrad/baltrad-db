#include <brfc/expr/Compiler.hpp>

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

#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>
#include <algorithm>

namespace brfc {
namespace expr {

std::string
Compiler::pop() {
    BRFC_ASSERT(!stack_.empty());
    std::string top = stack_.back();
    stack_.pop_back();
    return top;
}

void
Compiler::push(const std::string& top) {
    stack_.push_back(top);
}


void
Compiler::do_visit(BinaryOperator& expr) {
    std::string rhs = pop();
    std::string lhs = pop();
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
Compiler::do_visit(Literal& expr) {
    QString key = QString(":lit_") + QString::number(literal_count_++);
    push(key.toStdString());
    binds_[key] = expr.value();
}

void
Compiler::do_visit(Label& label) {
    push(pop() + " AS " + label.label());
}

void
Compiler::do_visit(Parentheses& expr) {
    push("(" + pop() + ")");
}

void
Compiler::do_visit(FromClause& from) {
    std::vector<std::string> from_clause_elements;
    for (size_t i = 0; i < from.elements().size(); ++i) {
        from_clause_elements.push_back(pop());
    }
    std::reverse(from_clause_elements.begin(), from_clause_elements.end());
    if (!from.empty()) {
        std::string from_clause = "\nFROM " + boost::join(from_clause_elements, ", ");
        push(from_clause);
    }
}

void
Compiler::do_visit(Select& select) {
    std::string where_clause;
    if (select.where())
        where_clause = "\nWHERE " + pop();
    std::string from_clause;
    if (!select.from()->empty())
        from_clause = pop();
    
    std::vector<std::string> result_column_elm;
    for (size_t i = 0; i < select.what().size(); ++i) {
        result_column_elm.push_back(pop());
    }
    std::reverse(result_column_elm.begin(), result_column_elm.end());
    std::string result_columns = boost::join(result_column_elm, ", ");

    std::string distinct = select.distinct() ? "DISTINCT " : "";
    // SELECT columns FROM from_obj WHERE where_clause
    std::string clause = "SELECT " + distinct
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
