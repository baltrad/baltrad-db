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

#ifndef BRFC_EXPR_PROC_DISPATCH_HPP
#define BRFC_EXPR_PROC_DISPATCH_HPP

#include <brfc/assert.hpp>
#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {
namespace proc {

template<typename Function>
typename Function::result_type
unary_dispatch(const Function& func, const sexp& args) {
    BRFC_ASSERT(args.is_list());
    BRFC_ASSERT(args.size() == 1);
    return expr::apply_visitor(func, args.front());
}

template<typename Function>
typename Function::result_type
binary_dispatch(const Function& func, const sexp& args) {
    BRFC_ASSERT(args.is_list());
    BRFC_ASSERT(args.size() == 2);
    expr::sexp::const_iterator it = args.begin();
    const sexp& arg1 = *it;
    ++it;
    const sexp& arg2 = *it;
    return expr::apply_visitor(func, arg1, arg2);
}

} // namespace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_DISPATCH_HPP
