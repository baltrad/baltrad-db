#include <brfc/expr/Literal.hpp>
#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace expr {

sexp
Literal::to_sexp() const {
    switch (value_.type()) {
        case Variant::NONE:
            return sexp();
        case Variant::STRING:
            return sexp(value_.string());
        case Variant::INT64:
            return sexp(value_.int64_());
        case Variant::DOUBLE:
            return sexp(value_.double_());
        case Variant::BOOL:
            return sexp(value_.bool_());
        default:
            return sexp(value_.to_string());
    }
}

}
}
