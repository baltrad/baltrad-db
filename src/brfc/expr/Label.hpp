#ifndef BRFC_EXPR_LABEL_HPP
#define BRFC_EXPR_LABEL_HPP

#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

/**
 * @brief labeled expression
 *
 * table.column AS label
 *
 * @ingroup exposed_in_binds
 */
class Label : public Expression {
  public:
    static LabelPtr create(ExpressionPtr expression, const QString& name) {
        return LabelPtr(new Label(expression, name));
    }

    void name(const QString& name) {
        name_ = name;
    }

    const QString& name() const {
        return name_;
    }

    void expression(ExpressionPtr expression) {
        expression_ = expression;
    }

    /**
     * @brief get expression this label is for
     */
    ExpressionPtr expression() const {
        return expression_;
    }

  protected:
    explicit Label(ExpressionPtr expression, const QString& name)
            : expression_(expression)
            , name_(name) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    ExpressionPtr expression_;
    QString name_;
};

}
}

#endif // BRFC_EXPR_LABEL_HPP
