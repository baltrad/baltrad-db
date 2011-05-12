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
#ifndef BRFC_EXPR_PROC_UNARY_LIST_HPP
#define BRFC_EXPR_PROC_UNARY_LIST_HPP

#include <boost/function.hpp>

namespace brfc {
namespace expr {

class Expression;

namespace proc {

/**
 * @brief wrap unary operator to support operations on a list
 */
struct unary_list {
    typedef boost::function<Expression(const Expression&)> proc_t;
    typedef Expression result_type;

    unary_list(proc_t proc) : proc_(proc) {}
    
    /**
     * @brief apply proc to all elements in args
     */
    Expression operator()(const Expression& args) const;

    proc_t proc_;
};

} // namepsace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_UNARY_LIST_HPP
