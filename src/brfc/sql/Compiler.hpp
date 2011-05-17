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

#ifndef BRFC_SQL_COMPILER_HPP
#define BRFC_SQL_COMPILER_HPP

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace sql {

/**
 * @brief ABC for SQL expression compiler
 */
class Compiler {
  public:
    /**
     * @brief compile @c x to sql form
     * @return list expression
     *
     * the return value is a list that contains string expressions and
     * expressions in form `(bind "bindname")` that are placeholders for
     * literals to be bound later.
     */
    Expression compile(const Expression& x) {
        return do_compile(x);
    }
    
    virtual ~Compiler() { }

  private:
    /**
     * @brief compile() implementaion
     */
    virtual Expression do_compile(const Expression& x) = 0;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_COMPILER_HPP
