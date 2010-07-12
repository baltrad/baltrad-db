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
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_RDB_COMPILER_HPP
#define BRFC_RDB_COMPILER_HPP

#include <brfc/assert.hpp>
#include <brfc/visit.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/BindMap.hpp>

#include <vector>

namespace brfc {

namespace sql {

/**
 * @brief compile elements to string form
 */
class Compiler {
  public:
    typedef mpl::vector<Alias,
                        BinaryOperator,
                        Column,
                        FromClause,
                        Join,
                        Parentheses,
                        Label,
                        Literal,
                        Select,
                        Table> accepted_types;

    /**
     * @brief constructor
     */
    Compiler()
            : in_from_clause_(false)
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
    void compile(T& expr);

    /**
     * @brief access literal bindings
     */
    const BindMap& binds() const { return binds_; }
    
    /**
     * @brief access to compiled string
     */
    const String& compiled() const { return stack_.back(); }

    /**
     * @brief compile Alias to string form
     *
     * @post top of the stack contains alias or 'content AS alias'
     */
    void operator()(Alias& expr);

    /**
     * @brief compile binary operator to string form
     *
     * @post stack contains lhs OP rhs
     */
    void operator()(BinaryOperator& expr);

    /**
     * @brief compile column to string form
     * @post top of the stack contains compiled column
     */
    void operator()(Column& expr);
    
    /**
     * @brief compile from clause to string form
     * @post top of the stack contains FROM + selectables joined by ','
     */
    void operator()(FromClause& expr);

    /**
     * @brief compile Join to string form
     * @post top of the stack contains 
     *       '\<from\> JOIN \<to\> ON \<expression\>'
     */
    void operator()(Join& join);

    /**
     * @brief compile Literal to string form
     * @post top of the stack contains literal binding key, binds contains
     *       a mapping of this key to value
     */
    void operator()(Literal& expr);
    
    /**
     * @brief compile Label to string form
     * @post top of the stack contains labelled AS label
     */
    void operator()(Label& label);

    /**
     * @brief surround the top of the stack with parentheses
     * @post stack contains (content)
     */
    void operator()(Parentheses& expr);

    /**
     * @brief compile Select statement to string form
     * @post top of the stack contains compiled where clause
     */
    void operator()(Select& select);

    /**
     * @brief compile Table to string form
     * @post top of the stack contains table name
     */
    void operator()(Table& expr);

  private:
    String pop();
    void push(const String& str);

    bool in_from_clause_;
    unsigned int literal_count_;
    BindMap binds_;
    std::vector<String> stack_;
};

}
}

#endif // BRFC_RDB_COMPILER_HPP
