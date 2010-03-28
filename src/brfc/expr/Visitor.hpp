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

#ifndef BRFC_EXPR_VISITOR_HPP
#define BRFC_EXPR_VISITOR_HPP

#include <brfc/expr/fwd.hpp>

namespace brfc {
namespace expr {

/**
 * @brief ABC for traversing SQL statement trees
 */
class Visitor {
  public:
    /**
     * @brief order of traversal
     */
    enum Order {
        PRE_ORDER = 1, ///< traverse parent first
        POST_ORDER = 2 ///< traverse children first
    };

    /**
     * @brief constructor
     * @param order how should this visitor traverse
     */
    explicit Visitor(Order order)
            : in_from_clause(false)
            , order_(order) {
    }
    
    /**
     * @brief destructor
     */
    virtual ~Visitor() { }

    void visit(Alias& alias);
    void visit(Attribute& attribute);
    void visit(Column& expr);
    void visit(FromClause& from);
    void visit(BinaryOperator& op);
    void visit(Join& join);
    void visit(Label& label);
    void visit(Literal& expr);
    void visit(Parentheses& expr);
    void visit(Table& table);
    /**
     * @brief visit Select statement
     *
     * visit what, from, where
     */
    void visit(Select& select);
  
  protected:
    virtual void do_visit(Alias& alias) = 0;
    virtual void do_visit(Attribute& attribute) = 0;
    virtual void do_visit(Column& expr) = 0;
    virtual void do_visit(FromClause& from) = 0;
    virtual void do_visit(BinaryOperator& op) = 0;
    virtual void do_visit(Join& join) = 0;
    virtual void do_visit(Label& label) = 0;
    virtual void do_visit(Literal& expr) = 0;
    virtual void do_visit(Parentheses& expr) = 0;
    virtual void do_visit(Table& table) = 0;
    virtual void do_visit(Select& select) = 0;

    bool in_from_clause; //XXX: hack

  private:
    Order order_;
};

}
}

#endif // BRFC_EXPR_VISITOR_HPP
