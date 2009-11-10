#include <brfc/expr/FromClause.hpp>

#include <brfc/expr/Selectable.hpp>

#include <boost/foreach.hpp>
#include <stdexcept>

namespace brfc {
namespace expr {

void
FromClause::add(SelectablePtr selectable) {
    if (has(selectable))
        throw std::runtime_error("duplicate selectable");
    elements_.push_back(selectable);
}

bool
FromClause::has(SelectablePtr selectable) const {
    // always accept unnamed
    if (selectable->name() == "")
        return false;
    BOOST_FOREACH(SelectablePtr element, elements_) {
        if (selectable->name() == element->name())
            return true;
    }
    return false;
}

} // namespace expr
} // namespace brfc
