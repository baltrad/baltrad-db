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

#ifndef BRFC_EXPR_PROC_LIST_FILTER_HPP
#define BRFC_EXPR_PROC_LIST_FILTER_HPP

#include <boost/function.hpp>

namespace brfc {
namespace expr {

class Expression;

namespace proc {

struct list_filter {
    typedef boost::function<Expression(const Expression&)> proc_t;
    typedef Expression result_type;

    list_filter(proc_t proc) : proc_(proc) {}

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
     * @return true if applying proc to rhs and any element in lhs returns true
     *
     * for each element e in lhs, call proc(e, rhs)
     */
    Expression left_list(const Expression& lhs,
                         const Expression& rhs) const;
    /**
     * @param lhs a plain expression
     * @param rhs a list expression
     * @return true if applying proc to lhs and any element in rhs returns true
     *
     * for each element e in rhs, call proc(lhs, e)
     */
    Expression right_list(const Expression& lhs,
                          const Expression& rhs) const;
    
    /**
     * @param lhs a list expression
     * @param rhs a list expression
     * @return true if applying proc to any combination of lhs and rhs element
     *         returns true
     */
    Expression both_list(const Expression& lhs,
                         const Expression& rhs) const;

    proc_t proc_;
};

} // namepsace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_LIST_FILTER_HPP
