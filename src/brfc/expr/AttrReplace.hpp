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
 * The strategy is to first replace all the attributes with columns,
 * collecting the unique tables/aliases. Since the Select has no from
 * clause, we determine a central table and join the remaining tables to it
 */
class AttrReplace : protected Visitor {
  public:
    /**
     * @brief replace attributes in a Select statement
     */
    static void replace(SelectPtr select, const AttributeMapper* mapper);
  
  protected:
    /**
     * @brief constructor
     * @param mapper AttributeMapper instance to fetch mappings from
     */
    AttrReplace(SelectPtr select, const AttributeMapper* mapper);

    /**
     * @brief visit an Alias element
     */
    virtual void do_visit(Alias& alias);

    /**
     * @brief visit an Attribute element
     *
     * replace this attribute with a column
     * if it's not a specialized column,
     * add a where clause to attribute names
     */
    virtual void do_visit(Attribute& attr);

    virtual void do_visit(Column& column);

    virtual void do_visit(BinaryOperator& op);

    virtual void do_visit(Join& join);

    virtual void do_visit(Label& label);

    virtual void do_visit(Literal& literal);

    virtual void do_visit(Parentheses& parentheses);

    virtual void do_visit(Table& table);

    virtual void do_visit(Select& select);

    virtual void do_visit(FromClause& from);

    void replace_attributes();

    void build_from_clause();

    ElementPtr pop();

    void push(ElementPtr p);

    void join_data_objects();

  private:
    const AttributeMapper* mapper_;
    std::vector<ElementPtr> stack_;
    SelectPtr select_;
    JoinPtr from_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_ATTR_REPLACE_HPP
