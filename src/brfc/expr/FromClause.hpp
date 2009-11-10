#ifndef BRFC_EXPR_FROM_CLAUSE_HPP
#define BRFC_EXPR_FROM_CLAUSE_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Element.hpp>
#include <vector>

namespace brfc {
namespace expr {

class FromClause : public Element {
  public:
    static FromClausePtr create() {
        return FromClausePtr(new FromClause());
    }

    void add(SelectablePtr selectable);

    bool has(SelectablePtr selectable) const;

    std::vector<SelectablePtr>& elements() { return elements_; }

    bool empty() const {
        return elements_.empty();
    }

  protected:
    FromClause()
            : elements_() {
    }

    virtual void do_accept(Visitor& visitor) { visitor.visit(*this); }

  private:
    std::vector<SelectablePtr> elements_;
};

}
}

#endif // BRFC_EXPR_FROM_CLAUSE_HPP
