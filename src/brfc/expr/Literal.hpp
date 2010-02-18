#ifndef BRFC_EXPR_LITERAL_HPP
#define BRFC_EXPR_LITERAL_HPP

#include <brfc/expr/Expression.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace expr {

/**
 * @brief a literal value
 *
 * @ingroup exposed_in_binds
 */
class Literal : public Expression {
  public:
    static LiteralPtr create(const Variant& value) {
        return LiteralPtr(new Literal(value));
    }

/*
  XXX: this seems problematic, we need to be able to convert
       any brfc::Variant to string for this to work, but this
       is not implemented.

       This method seems not to be used anywhere, so, remove?.

    String string() const {
        return value_.string();
    }
*/
    const Variant& value() const {
        return value_;
    }

  protected:
    explicit Literal(const Variant& value)
            : value_(value) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    Variant value_;
};

}
}

#endif // BRFC_EXPR_LITERAL_HPP
