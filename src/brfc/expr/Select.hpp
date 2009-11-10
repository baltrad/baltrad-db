#ifndef BRFC_EXPR_SELECT_HPP
#define BRFC_EXPR_SELECT_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Selectable.hpp>
#include <vector>

namespace brfc {
namespace expr {

class Select : public Selectable {
  public:
    virtual std::string name() const { return ""; }

    static SelectPtr create() {
        return SelectPtr(new Select());
    }

    void what(ExpressionPtr expr) {
        what_.push_back(expr);
    }

    void from(FromClausePtr from) { from_ = from; }
    
    /**
     * @brief want unique results
     */
    void distinct(bool distinct) {
        distinct_ = distinct;
    }
    
    /**
     * @brief is distinct set?
     */
    bool distinct() const {
        return distinct_;
    }
    
    /**
     * @brief append Expression to where clause
     */
    void append_where(ExpressionPtr expr);

    /**
     * @brief set Expression as where clause
     */
    void where(ExpressionPtr expr) {
        where_ = expr;
    }

    const std::vector<ExpressionPtr>& what() const {
        return what_;
    }

    FromClausePtr from() const {
        return from_;
    }


    ExpressionPtr where() const {
        return where_;
    }

    std::vector<ExpressionPtr>& what() {
        return what_;
    }

    FromClausePtr from() {
        return from_;
    }


    ExpressionPtr where() {
        return where_;
    }

  
  protected:
    Select();

    virtual void do_accept(Visitor& visitor); 

  private:
    std::vector<ExpressionPtr> what_;
    FromClausePtr from_;
    ExpressionPtr where_;
    bool distinct_;
};

}
}

#endif // BRFC_EXPR_SELECT_HPP
