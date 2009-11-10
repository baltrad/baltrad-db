#ifndef BRFC_EXPR_LITERAL_HPP
#define BRFC_EXPR_LITERAL_HPP

#include <brfc/expr/Expression.hpp>

#include <QtCore/QVariant>

namespace brfc {
namespace expr {

/**
 * @brief a literal value
 *
 * @ingroup exposed_in_binds
 */
class Literal : public Expression {
  public:
    static LiteralPtr create(const QVariant& value) {
        return LiteralPtr(new Literal(value));
    }

    std::string string() const {
        return value_.toString().toStdString();
    }

    const QVariant& value() const {
        return value_;
    }

  protected:
    explicit Literal(const QVariant& value)
            : value_(value) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    QVariant value_;
};

}
}

#endif // BRFC_EXPR_LITERAL_HPP
