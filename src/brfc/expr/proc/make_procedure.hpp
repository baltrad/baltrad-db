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
#ifndef BRFC_EXPR_PROC_MAKE_PROCEDURE_HPP
#define BRFC_EXPR_PROC_MAKE_PROCEDURE_HPP

#include <string>

namespace brfc {
    class Expression;
}

namespace brfc {
namespace proc {

/**
 * @brief constructs a procedure expression
 */
class make_procedure {
  public:
    typedef Expression result_type;
    
    /**
     * @param name name of the procedure
     * @param nargs number of arguments, -1 for any (the default)
     */
    make_procedure(const std::string& name, int nargs=-1);
    
    /**
     * @param x arguments to the procedure
     * @return (name, arg1, arg2, ... argN)
     * @throw logic_error if number of arguments in x doesn't match nargs
     */
    Expression operator()(const Expression& x) const;

  private:
    std::string name_;
    int nargs_;
};

} // namespace proc
} // namespace brfc

#endif // BRFC_EXPR_PROC_MAKE_PROCEDURE_HPP
