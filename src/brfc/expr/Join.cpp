#include <brfc/expr/Join.hpp>

#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

Join::Join(SelectablePtr from,
           SelectablePtr to,
           ExpressionPtr condition,
           Type type)
        : from_(from)
        , to_(to)
        , condition_(condition)
        , type_(type) {

}

void
Join::do_accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool
Join::contains(SelectablePtr element) const {
    int contains = 0;

    if (JoinPtr j = dynamic_pointer_cast<Join>(from_)) {
        contains += j->contains(element);
    } else {
        contains += from_->name() == element->name();
    }

    if (JoinPtr j = dynamic_pointer_cast<Join>(to_)) {
        contains += j->contains(element);
    } else {
        contains += to_->name() == element->name();
    }

    return contains;
}

} // namespace expr
} // namespace brfc
