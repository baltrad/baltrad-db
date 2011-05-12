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

#ifndef BRFC_EXPR_PROC_BINARY_LIST_HPP
#define BRFC_EXPR_PROC_BINARY_LIST_HPP

#include <boost/function.hpp>

namespace brfc {
namespace expr {

class Expression;

namespace proc {

/**
 * @brief wrap binary operator to support operations on lists
 */
struct binary_list {
    typedef boost::function<Expression(const Expression&)> proc_t;
    typedef Expression result_type;

    binary_list(proc_t proc) : proc_(proc) {}
    
    /**
     * @param args `(lhs, rhs)`
     * @return depends on the arguments
     * 
     * depending on the type of lhs and rhs forwards to:
     *  - @ref left_list() if lhs is a list and rhs is not a list
     *  - @ref right_list() if lhs is not a list and rhs is a list
     *  - @ref both_list() if lhs is not a list and rhs is a list
     *  - calls proc on lhs and rhs
     */
    Expression operator()(const Expression& args) const;
    
    /**
     * @param lhs a list expression
     * @param rhs a plain expression
     * @return list of results applying proc to elements in lhs and rhs
     *
     * for each element e in lhs, call proc(e, rhs)
     */
    Expression left_list(const Expression& lhs,
                         const Expression& rhs) const;
    /**
     * @param lhs a plain expression
     * @param rhs a list expression
     * @return list of results applying proc lhs and elements in rhs
     *
     * for each element e in rhs, call proc(lhs, e)
     */
    Expression right_list(const Expression& lhs,
                          const Expression& rhs) const;
    
    /**
     * @param lhs a list expression
     * @param rhs a list expression
     * @return list of results applying proc to elements in lhs and
     *         elements in rhs
     *
     * for each element r in rhs, and l in lhs call proc(l, r)
     */
    Expression both_list(const Expression& lhs,
                         const Expression& rhs) const;

    proc_t proc_;
};

} // namepsace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_BINARY_LIST_HPP
