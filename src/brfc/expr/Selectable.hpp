#ifndef BRFC_EXPR_SELECTABLE_HPP
#define BRFC_EXPR_SELECTABLE_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Element.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

class Selectable : public Element {
  public:
    SelectablePtr shared_from_this() const {
        return static_pointer_cast<Selectable>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }

    virtual QString name() const = 0;

    ColumnPtr column(const QString& name);

    AliasPtr alias(const QString& name);

    JoinPtr join(SelectablePtr rhs, ExpressionPtr condition);
};

}
}

#endif // BRFC_EXPR_SELECTABLE_HPP
