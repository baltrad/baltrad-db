#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {

sexp
Parentheses::to_sexp() const {
    return expr_->to_sexp(); // XXX: ignore this
}

}
}
