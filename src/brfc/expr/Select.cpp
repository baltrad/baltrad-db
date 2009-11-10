#include <brfc/expr/Select.hpp>

#include <brfc/expr/Expression.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/FromClause.hpp>

#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

Select::Select()
        : what_()
        , from_(FromClause::create())
        , where_()
        , distinct_(false) {
}


void
Select::append_where(ExpressionPtr expr) {
    where_ = where_ ? where_->and_(expr) : expr;
}

void
Select::do_accept(Visitor& visitor) {
    visitor.visit(*this);
} 

} // namespace expr
} // namespace brfc
