#ifndef BRFC_EXPR_JOIN_HPP
#define BRFC_EXPR_JOIN_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Selectable.hpp>

namespace brfc {
namespace expr {

class Join : public Selectable {
  public:
    static JoinPtr create(SelectablePtr from,
                          SelectablePtr to,
                          ExpressionPtr condition) {
        return JoinPtr(new Join(from, to, condition));
    }

    virtual QString name() const { return ""; }

    SelectablePtr from() const {
        return from_;
    }

    void from(SelectablePtr from) {
        from_ = from;
    }

    SelectablePtr to() const {
        return to_;
    }

    void to(SelectablePtr to) {
        to_ = to;
    }

    ExpressionPtr condition() const {
        return condition_;
    }

    void condition(ExpressionPtr condition) {
        condition_ = condition;
    }
    
    /**
     * @brief test if join contains a selectable element
     *
     * if this join is part of a chain, the check is made in the entire
     * chain. Selectable::name is used to determine equality.
     */
    bool contains(SelectablePtr element) const;

  protected:
    Join(SelectablePtr from, SelectablePtr to, ExpressionPtr condition);

    virtual void do_accept(Visitor& visitor);

  private:
    SelectablePtr from_;
    SelectablePtr to_;
    ExpressionPtr condition_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_JOIN_HPP
