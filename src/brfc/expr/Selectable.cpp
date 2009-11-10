#include <brfc/expr/Selectable.hpp>
#include <brfc/expr/Alias.hpp>
#include <brfc/expr/Column.hpp>

namespace brfc {
namespace expr {

AliasPtr
Selectable::alias(const std::string& name) {
    return Alias::create(this->shared_from_this(), name);
}

ColumnPtr
Selectable::column(const std::string& name) {
    return Column::create(this->shared_from_this(), name);
}

} // namespace expr
} // namespace brfc
