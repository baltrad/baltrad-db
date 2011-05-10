/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_SQL_DIALECT_COMPILER_HPP
#define BRFC_SQL_DIALECT_COMPILER_HPP

#include <vector>

#include <brfc/expr/Eval.hpp>
#include <brfc/sql/Compiler.hpp>

namespace brfc {
namespace sql {

class Dialect;

/**
 * @brief compiler using a Dialect
 */
class DialectCompiler : public Compiler {
  public:
    struct proc {
        typedef expr::Expression result_type;
    };

    /**
     * @brief constructor
     */
    explicit DialectCompiler(const Dialect* dialect);

    /**
     * @brief compile Alias to string form
     *
     * @post top of the stack contains alias or 'aliased AS alias' if compiling
     *       a FROM-clause
     */
    struct alias : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief binary operator
     */
    struct binop : public proc {
        binop(const std::string& op) : op_(op) { }
        
        expr::Expression operator()(const expr::Expression& args);

        std::string op_;
    };

    struct like : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };
    
    struct unaryop : public proc {
        unaryop(const std::string& op) : op_(op) { }

        expr::Expression operator()(const expr::Expression& x);

        std::string op_;
    };

    /**
     * @brief compile a Bind to string form
     *
     * @post top of the stack constains bind name, BindMap has an entry
     *       with the name and null value
     */
    struct bind : public proc {
        expr::Expression operator()(const expr::Expression& x);
        
        BindMap binds_;
    };

    /**
     * @brief compile column to string form
     * @post top of the stack contains compiled column
     */
    struct column : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief compile function call to string form
     * @post top of the stack contains 'name(arg1, arg2, ...)'
     */
    struct function : public proc {
        explicit function(const std::string& name) : name_(name) { }

        expr::Expression operator()(const expr::Expression& x);

        std::string name_;
    };
    
    /**
     * @brief compile Join to string form
     * @post top of the stack contains 
     *       '\<from\> JOIN \<to\> ON \<expression\>'
     */
    struct join : public proc {
        explicit join(const std::string& type)
                : type_(type) {
        }

        expr::Expression operator()(const expr::Expression& x);

        std::string type_;
    };

    /**
     * @brief compile Literal to string form
     * @post top of the stack contains result of Dialect::variant_to_string
     */
    struct literal : public proc {
        literal(const sql::Dialect& dialect) : dialect_(dialect) { }

        expr::Expression operator()(const expr::Expression& x);

        const sql::Dialect& dialect_;
    };

    /**
     * @brief compile Label to string form
     * @post top of the stack contains 'expression AS name'
     */
    struct label : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief compile Select statement to string form
     * @post top of the stack contains 'SELECT what FROM from WHERE where'
     */
    struct select : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct distinct : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct select_columns : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct from_clause : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct where_clause : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct group_by : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct order_by : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct order_dir : public proc {
        explicit order_dir(const std::string& dir) : dir_(dir) { }

        expr::Expression operator()(const expr::Expression& x);

        std::string dir_;
    };

    struct limit : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct offset : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief compile Insert statement to string form
     * @post top of the stack contains 'INSERT INTO table(bind.keys)
     *       VALUES(bind.values) RETURNING returns'
     */
    struct insert : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct insert_columns : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    struct insert_values : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };
    
    struct returning : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief compile Table to string form
     * @post top of the stack contains table name
     */
    struct table : public proc {
        expr::Expression operator()(const expr::Expression& x);
    };

    expr::Expression compact_str(const expr::Expression& x);
  
  protected:
    /**
     * @brief compile expression/statement to string form
     * @pre stack is empty
     * @post stack contains compiled string, accessible through compiled()
     */
    virtual Query do_compile(const expr::Expression& x);

  private:
    expr::Eval eval_;
    const Dialect* dialect_;
    bind bind_;
};

}
}

#endif // BRFC_SQL_DIALECT_COMPILER_HPP
