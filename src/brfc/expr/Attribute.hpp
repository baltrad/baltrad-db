#ifndef BRFC_EXPR_ATTRIBUTE_HPP
#define BRFC_EXPR_ATTRIBUTE_HPP

#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Visitor.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

/**
 * @brief placeholder for HDF5 attributes
 * @ingroup exposed_in_binds
 */
class Attribute : public Expression {
  public:
    static AttributePtr create(const QString& name) {
        return AttributePtr(new Attribute(name));
    }

    const QString& name() const {
        return name_;
    }

  protected:
    Attribute(const QString& name)
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

#endif // BRFC_EXPR_ATTRIBUTE_HPP
