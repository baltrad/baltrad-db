#include <brfc/expr/Function.hpp>

#include <boost/foreach.hpp>

#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace expr {

namespace {
std::string
to_string(Function::Name name) {
    switch (name) {
        case Function::COUNT: return "count";
        case Function::MAX: return "max";
        case Function::MIN: return "min";
        case Function::SUM: return "sum";
    }
}

}

sexp
Function::to_sexp() const {
    listcons lc;
    lc.symbol(to_string(name_));
    BOOST_FOREACH(ExpressionPtr expr, args_) {
        lc.append(expr->to_sexp());
    }
    return lc.get();
}

}
}
