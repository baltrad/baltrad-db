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

#ifndef BRFC_SQL_DEFAULT_COMPILER_HPP
#define BRFC_SQL_DEFAULT_COMPILER_HPP

#include <brfc/visit.hpp>
#include <brfc/Variant.hpp>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Compiler.hpp>

#include <vector>

namespace brfc {
namespace sql {

/**
 * @brief default Compiler implementation
 */
class DefaultCompiler : public Compiler {
  public:
    typedef mpl::vector<const Alias,
                        const BinaryOperator,
                        const Column,
                        const Function,
                        const Insert,
                        const Join,
                        const Parentheses,
                        const Label,
                        const Literal,
                        const Select,
                        const Table> accepted_types;

    /**
     * @brief constructor
     */
    DefaultCompiler()
            : in_from_clause_(false)
            , literal_count_(0)
            , binds_()
            , stack_() {
    }

    /**
     * @brief compile Alias to string form
     *
     * @post top of the stack contains alias or 'aliased AS alias' if compiling
     *       a FROM-clause
     */
    void operator()(const Alias& expr);

    /**
     * @brief compile binary operator to string form
     *
     * @post stack contains 'lhs op rhs'
     */
    void operator()(const BinaryOperator& expr);

    /**
     * @brief compile column to string form
     * @post top of the stack contains compiled column
     */
    void operator()(const Column& expr);

    /**
     * @brief compile function call to string form
     * @post top of the stack contains 'name(arg1, arg2, ...)'
     */
    void operator()(const Function& func);
    
    /**
     * @brief compile Join to string form
     * @post top of the stack contains 
     *       '\<from\> JOIN \<to\> ON \<expression\>'
     */
    void operator()(const Join& join);

    /**
     * @brief compile Literal to string form
     * @post top of the stack contains literal binding key, binds contains
     *       a mapping of this key to value
     */
    void operator()(const Literal& expr);
    
    /**
     * @brief compile Label to string form
     * @post top of the stack contains 'expression AS name'
     */
    void operator()(const Label& label);

    /**
     * @brief surround the top of the stack with parentheses
     * @post stack contains '(content)'
     */
    void operator()(const Parentheses& expr);

    /**
     * @brief compile Select statement to string form
     * @post top of the stack contains 'SELECT what FROM from WHERE where'
     */
    void operator()(const Select& select);

    /**
     * @brief compile Insert statement to string form
     * @post top of the stack contains 'INSERT INTO table(bind.keys)
     *       VALUES(bind.values) RETURNING returns'
     */
    void operator()(const Insert& insert);

    /**
     * @brief compile Table to string form
     * @post top of the stack contains table name
     */
    void operator()(const Table& expr);
  
  protected:
    /**
     * @brief compile expression/statement to string form
     * @pre stack is empty
     * @post stack contains compiled string, accessible through compiled()
     */
    virtual Query do_compile(const Element& expr);

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

#endif // BRFC_SQL_DEFAULT_COMPILER_HPP
