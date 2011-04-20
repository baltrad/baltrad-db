#include <brfc/expr/BinaryOperator.hpp>

#include <stdexcept>

#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace expr {

namespace {

std::string
to_string(BinaryOperator::Type type) {
    switch (type) {
        case BinaryOperator::NE: return "!=";
        case BinaryOperator::EQ: return "=";
        case BinaryOperator::GT: return ">";
        case BinaryOperator::LT: return "<";
        case BinaryOperator::LE: return "<=";
        case BinaryOperator::GE: return ">=";
        case BinaryOperator::LIKE: return "like";
        case BinaryOperator::IN: return "in";
        case BinaryOperator::NOT_IN: return "not_in";
        case BinaryOperator::AND: return "and";
        case BinaryOperator::OR: return "or";
        case BinaryOperator::ADD: return "+";
        case BinaryOperator::SUB: return "-";
        case BinaryOperator::MUL: return "*";
        case BinaryOperator::DIV: return "/";
    }
    throw std::runtime_error("");
}

}

sexp
BinaryOperator::to_sexp() const {
    return listcons().symbol(to_string(type_))
                     .append(lhs_->to_sexp())
                     .append(rhs_->to_sexp())
                     .get();
}

}
}
