/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_EXPR_COMPILER_HPP
#define BRFC_EXPR_COMPILER_HPP

#include <brfc/assert.hpp>
#include <brfc/Variant.hpp>
#include <brfc/expr/Visitor.hpp>


#include <map>
#include <string>
#include <vector>

namespace brfc {
namespace expr {

/**
 * @brief compile elements to string form
 */
class Compiler : public Visitor {
  public:
    typedef std::map<QString, Variant> BindMap;

    /**
     * @brief constructor
     */
    Compiler()
            : Visitor(Visitor::POST_ORDER)
            , literal_count_(0)
            , binds_()
            , stack_() {
    }

    /**
     * @brief compile expression/statement to string form
     * @pre stack is empty
     * @post stack contains compiled string, accessible through compiled()
     */
    template<typename T>
    void compile(T& expr) {
        BRFC_ASSERT(stack_.empty());
        expr.accept(*this);
        BRFC_ASSERT(stack_.size() == 1);
    }
    
    /**
     * @brief access literal bindings
     */
    const BindMap& binds() const { return binds_; }
    
    /**
     * @brief access to compiled string
     */
    const QString& compiled() const { return stack_.back(); }

  protected:
    /**
     * @brief compile Alias to string form
     *
     * @pre top of the stack contains aliased content
     * @post top of the stack contains alias or 'content AS alias'
     */
    virtual void do_visit(Alias& expr);

    /**
     * @brief always throws
     *
     * @throw assert_error (attributes should be replaced before compiling)
     */
    virtual void do_visit(Attribute& expr);

    /**
     * @brief compile binary operator to string form
     *
     * @pre stack contains rhs and lhs
     * @post stack contains lhs OP rhs
     */
    virtual void do_visit(BinaryOperator& expr);

    /**
     * @brief compile column to string form
     * @pre top of the stack contains compiled Selectable
     * @post top of the stack contains compiled column
     */
    virtual void do_visit(Column& expr);
    
    /**
     * @brief compile from clause to string form
     * @pre stack size >= from clause element count
     * @post top of the stack contains FROM + selectables joined by ','
     */
    virtual void do_visit(FromClause& expr);

    /**
     * @brief compile Join to string form
     * @pre stack contains expression, to and from in string form
     * @post top of the stack contains 
     *       '\<from\> JOIN \<to\> ON \<expression\>'
     */
    virtual void do_visit(Join& join);

    /**
     * @brief compile Literal to string form
     * @post top of the stack contains literal binding key, binds contains
     *       a mapping of this key to value
     */
    virtual void do_visit(Literal& expr);
    
    /**
     * @brief compile Label to string form
     * @pre top of the stack contains labelled in string form
     * @post top of the stack contains labelled AS label
     */
    virtual void do_visit(Label& label);

    /**
     * @brief surround the top of the stack with parentheses
     * @pre top of the stack contains parentheses content
     * @post stack contains (content)
     */
    virtual void do_visit(Parentheses& expr);

    /**
     * @brief compile Select statement to string form
     * @pre stack contains compiled where clause, compiled from clause
     *      and at least as much elements as there are elements in
     *      result columns
     * @post top of the stack contains compiled where clause
     */
    virtual void do_visit(Select& select);

    /**
     * @brief compile Table to string form
     * @post top of the stack contains table name
     */
    virtual void do_visit(Table& expr);

  private:
    QString pop();
    void push(const QString& str);

    unsigned int literal_count_;
    BindMap binds_;
    std::vector<QString> stack_;
};

}
}

#endif // BRFC_EXPR_COMPILER_HPP
