#ifndef BRFC_EXPR_TABLE_HPP
#define BRFC_EXPR_TABLE_HPP

#include <brfc/expr/Selectable.hpp>

namespace brfc {
namespace expr {

class Table : public Selectable {
  public:
    static TablePtr create(const QString& name) {
        return TablePtr(new Table(name));
    }

    void name(const QString& name) {
        name_ = name;
    }

    virtual QString name() const {
        return name_;
    }

  protected:
    explicit Table(const QString& name)
            : name_(name) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }
  
  private:
    QString name_;
};

}
}

#endif // BRFC_EXPR_TABLE_HPP
