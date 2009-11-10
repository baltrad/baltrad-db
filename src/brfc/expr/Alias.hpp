#ifndef BRFC_EXPR_ALIAS_HPP
#define BRFC_EXPR_ALIAS_HPP

#include <brfc/expr/Selectable.hpp>
#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

class Alias : public Selectable {
  public:
    static AliasPtr create(SelectablePtr aliased,
                           const std::string& alias) {
        return AliasPtr(new Alias(aliased, alias));
    }

    void aliased(SelectablePtr aliased) {
        aliased_ = aliased;
    }

    SelectablePtr aliased() const {
        return aliased_;
    }

    void alias(const std::string& alias) {
        alias_ = alias;
    }

    const std::string& alias() const {
        return alias_;
    }

    virtual std::string name() const {
        return alias_;
    }

  protected:
    Alias(SelectablePtr aliased,
          const std::string& alias)
            : aliased_(aliased)
            , alias_(alias) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    SelectablePtr aliased_;
    std::string alias_;
};

}
}

#endif // BRFC_EXPR_ALIAS_HPP
