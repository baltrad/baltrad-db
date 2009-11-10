#ifndef BRFC_EXPR_TABLE_HPP
#define BRFC_EXPR_TABLE_HPP

#include <brfc/expr/Selectable.hpp>

namespace brfc {
namespace expr {

class Table : public Selectable {
  public:
    static TablePtr create(const std::string& name) {
        return TablePtr(new Table(name));
    }

    void name(const std::string& name) {
        name_ = name;
    }

    virtual std::string name() const {
        return name_;
    }

  protected:
    explicit Table(const std::string& name)
            : name_(name) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }
  
  private:
    std::string name_;
};

}
}

#endif // BRFC_EXPR_TABLE_HPP
