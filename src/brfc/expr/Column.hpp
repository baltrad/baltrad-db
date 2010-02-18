#ifndef BRFC_EXPR_COLUMN_HPP
#define BRFC_EXPR_COLUMN_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

class Column : public Expression {
  public:
    static ColumnPtr create(SelectablePtr selectable,
                            const QString& name) {
        return ColumnPtr(new Column(selectable, name));
    }

    void selectable(SelectablePtr selectable) {
        selectable_ = selectable;
    }

    SelectablePtr selectable() const { return selectable_; }

    void name(const QString& name) {
        name_ = name;
    }

    const QString& name() const { return name_; }

  protected:
    Column(SelectablePtr selectable,
           const QString& name)
            : selectable_(selectable)
            , name_(name) {

    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    SelectablePtr selectable_;
    QString name_;
};

}
}

#endif // BRFC_EXPR_COLUMN_HPP
