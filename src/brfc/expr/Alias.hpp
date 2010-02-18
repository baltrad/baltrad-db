#ifndef BRFC_EXPR_ALIAS_HPP
#define BRFC_EXPR_ALIAS_HPP

#include <brfc/expr/Selectable.hpp>
#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

class Alias : public Selectable {
  public:
    static AliasPtr create(SelectablePtr aliased,
                           const QString& alias) {
        return AliasPtr(new Alias(aliased, alias));
    }

    void aliased(SelectablePtr aliased) {
        aliased_ = aliased;
    }

    SelectablePtr aliased() const {
        return aliased_;
    }

    void alias(const QString& alias) {
        alias_ = alias;
    }

    const QString& alias() const {
        return alias_;
    }

    virtual QString name() const {
        return alias_;
    }

  protected:
    Alias(SelectablePtr aliased,
          const QString& alias)
            : aliased_(aliased)
            , alias_(alias) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    SelectablePtr aliased_;
    QString alias_;
};

}
}

#endif // BRFC_EXPR_ALIAS_HPP
