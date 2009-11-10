#ifndef BRFC_EXPR_ELEMENT_HPP
#define BRFC_EXPR_ELEMENT_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/expr/Visitor.hpp>
#include <boost/noncopyable.hpp>

namespace brfc {
namespace expr {

class Visitor;

/**
 * @brief ABC for elements in sql statements
 */
class Element : public boost::noncopyable
              , public enable_shared_from_this<Element> {
  public:
    virtual ~Element() {}
    
    void accept(Visitor& visitor) { do_accept(visitor); }

  protected:
    Element()
            : boost::noncopyable()
            , enable_shared_from_this<Element>() {
    }

    virtual void do_accept(Visitor& visitor) = 0;
};


}
}

#endif // BRFC_EXPR_ELEMENT_HPP
