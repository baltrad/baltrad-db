#include <brfc/expr/Selectable.hpp>
#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Column.hpp>
#include <brfc/expr/Join.hpp>

namespace brfc {
namespace expr {

AliasPtr
Selectable::alias(const QString& name) {
    return Alias::create(this->shared_from_this(), name);
}

ColumnPtr
Selectable::column(const QString& name) {
    return Column::create(this->shared_from_this(), name);
}

JoinPtr
Selectable::join(SelectablePtr rhs, ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::INNER);
}

JoinPtr
Selectable::outerjoin(SelectablePtr rhs, ExpressionPtr condition) {
    return Join::create(this->shared_from_this(), rhs, condition, Join::LEFT);
}

} // namespace expr
} // namespace brfc
