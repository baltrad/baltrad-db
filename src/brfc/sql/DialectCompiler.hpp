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
    /**
     * @brief constructor
     */
    explicit DialectCompiler(const Dialect* dialect);

    /**
     * @brief base for DialectCompiler procedures
     */
    struct proc {
        typedef expr::Expression result_type;
    };

    /**
     * @brief sql alias
     */
    struct alias : public proc {
        /**
         * @param x `(aliased, alias)`
         * @return `(aliased, " AS " + alias)` or
         *         `("(", aliased, ") AS " + alias)`
         *         if aliased is a SELECT-statement
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql binary operator
     */
    struct binop : public proc {
        /**
         * @param param op_ the operator string
         */
        binop(const std::string& op) : op_(op) { }
        
        /**
         * @param x `(lhs, rhs)`
         * @return `(lhs, " " + op + " ", rhs)`
         */
        expr::Expression operator()(const expr::Expression& args);

        std::string op_;
    };
    
    /**
     * @brief sql like operator
     */
    struct like : public proc {
        /**
         * @param x `(arg1, pattern)`
         * @return `(arg1, "LIKE " + pattern)`
         *
         * replaces '*' with '%' and '?' with '_' in the pattern
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql unary operator
     */
    struct unaryop : public proc {
        /**
         * @param op the operator string
         */
        unaryop(const std::string& op) : op_(op) { }
        
        /**
         * @param x `(arg1)`
         * @return `(op, arg1)`
         */
        expr::Expression operator()(const expr::Expression& x);

        std::string op_;
    };

    /**
     * @brief a placeholder for a literal bound on execution
     */
    struct bind : public proc {
        /**
         * @param x `(arg1)`
         * @return `(bind, arg1)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql column
     */
    struct column : public proc {
        /**
         * @param x `(table, column)`
         * @return `table + "." + column`
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql function call
     */
    struct function : public proc {
        /**
         * @param name the function name
         */
        explicit function(const std::string& name) : name_(name) { }
        
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `(name + "(", arg1, ", ", arg2, ", ", ... , argN, ")")`
         */
        expr::Expression operator()(const expr::Expression& x);

        std::string name_;
    };
    
    /**
     * @brief sql join clause
     */
    struct join : public proc {
        /**
         * @param type join type name
         */
        explicit join(const std::string& type)
                : type_(type) {
        }
        
        /**
         * @param x `(selectable [, condition])
         * @return `(type + " ", selectable [, " ON ", condition])`
         */
        expr::Expression operator()(const expr::Expression& x);

        std::string type_;
    };

    /**
     * @brief sql literal value
     */
    struct literal : public proc {
        /**
         * @param dialect the dialect used
         */
        literal(const sql::Dialect& dialect) : dialect_(dialect) { }
    
        /**
         * @param x `(arg)`
         * @return arg
         *
         * calls @ref Dialect::literal_to_string() on @c arg
         */
        expr::Expression operator()(const expr::Expression& x);

        const sql::Dialect& dialect_;
    };

    /**
     * @brief sql label
     */
    struct label : public proc {
        /**
         * @param x `(labeled, label)`
         * @return `(labeled, " AS " + label)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql SELECT statement
     */
    struct select : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("SELECT ", arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql distinct query marker
     */
    struct distinct : public proc {
        /**
         * @param x `()`
         * @return `"DISTINCT"`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select columns
     */
    struct select_columns : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `(arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select FROM-clause
     */
    struct from_clause : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("FROM ", arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select WHERE-clause
     */
    struct where_clause : public proc {
        /**
         * @param x `(condition)`
         * @return `("WHERE ", condition)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select GROUP BY clause
     */
    struct group_by : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("GROUP BY ", arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select ORDER BY clause
     */
    struct order_by : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("ORDER BY ", arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql ORDER BY clause direction indicator
     */
    struct order_dir : public proc {
        /**
         * @param dir the direction name
         */
        explicit order_dir(const std::string& dir) : dir_(dir) { }
        
        /**
         * @param x `(arg)`
         * @return `(arg, " " + dir)`
         */
        expr::Expression operator()(const expr::Expression& x);

        std::string dir_;
    };
    
    /**
     * @brief sql select limit
     */
    struct limit : public proc {
        /**
         * @param x `(n)`
         * @return `"LIMIT " + n`
         *
         * the (int64) argument is cast to string using boost::lexical_cast
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief sql select offset
     */
    struct offset : public proc {
        /**
         * @param x `(n)`
         * @return `"OFFSET " + n`
         *
         * the (int64) argument is cast to string using boost::lexical_cast
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql INSERT statement
     */
    struct insert : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("INSERT INTO ", arg1, " ", arg2, " ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief insert columns
     */
    struct insert_columns : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `arg1 + ", " + arg2 + ", " + ... + argN`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief insert values
     */
    struct insert_values : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("VALUES (", arg1, ", ", arg2, ", ", ..., argN, ")")`
         */
        expr::Expression operator()(const expr::Expression& x);
    };
    
    /**
     * @brief returning clause
     */
    struct returning : public proc {
        /**
         * @param x `(arg1, arg2, ..., argN)`
         * @return `("RETURNING ", arg1, ", ", arg2, ", ", ..., argN)`
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    /**
     * @brief sql table
     */
    struct table : public proc {
        /**
         * @param x `(table)`
         * @return `table`
         */
        expr::Expression operator()(const expr::Expression& x);
    };

    expr::Expression compact_str(const expr::Expression& x);
  
  private:
    /**
     * @brief compile expression/statement to string form
     * @pre stack is empty
     * @post stack contains compiled string, accessible through compiled()
     */
    virtual expr::Expression do_compile(const expr::Expression& x);

    expr::Eval eval_;
    const Dialect* dialect_;
    bind bind_;
};

}
}

#endif // BRFC_SQL_DIALECT_COMPILER_HPP
