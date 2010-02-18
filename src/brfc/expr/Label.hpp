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
    static LabelPtr create(ExpressionPtr expression, const QString& label) {
        return LabelPtr(new Label(expression, label));
    }

    void label(const QString& label) {
        label_ = label;
    }

    const QString& label() const {
        return label_;
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
    explicit Label(ExpressionPtr expression, const QString& label)
            : expression_(expression)
            , label_(label) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    ExpressionPtr expression_;
    QString label_;
};

}
}

#endif // BRFC_EXPR_LABEL_HPP
