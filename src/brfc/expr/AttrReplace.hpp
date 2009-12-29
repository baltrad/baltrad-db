#ifndef BRFC_EXPR_ATTR_REPLACE_HPP
#define BRFC_EXPR_ATTR_REPLACE_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Visitor.hpp>

#include <vector>

namespace brfc {

class AttributeMapper;

namespace expr {

/**
 * @brief replace expr::Attribute with expr::Column
 *
 * XXX: come up with a better name for this class to reflect what
 *      it is actually doing
 */
class AttrReplace : protected Visitor {
  public:
    /**
     * @brief constructor
     * @param mapper AttributeMapper instance to fetch mappings from
     */
    AttrReplace(const AttributeMapper* mapper);

    /**
     * @brief replace attributes in a Select statement
     */
    void replace(SelectPtr select);
  
  protected:
    /**
     * @brief visit an Alias element
     */
    virtual void do_visit(brfc::expr::Alias& alias);

    /**
     * @brief visit an Attribute element
     *
     * replace this attribute with a column
     * if it's not a specialized column,
     * add a where clause to attribute names
     */
    virtual void do_visit(brfc::expr::Attribute& attr);

    virtual void do_visit(Column& column);

    virtual void do_visit(BinaryOperator& op);

    virtual void do_visit(Label& label);

    virtual void do_visit(Literal& literal);

    virtual void do_visit(Parentheses& parentheses);

    virtual void do_visit(Table& table);

    virtual void do_visit(Select& select);

    virtual void do_visit(FromClause& from);

    void reset();

    void join_where(ExpressionPtr where);

    void join();

    ElementPtr pop();

    void push(ElementPtr p);

  private:
    const AttributeMapper* mapper_;
    std::vector<ElementPtr> stack_;
    SelectPtr select_;
    ExpressionPtr join_where_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_ATTR_REPLACE_HPP
