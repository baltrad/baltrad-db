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

#ifndef BRFC_EXPR_PROC_LIKE_HPP
#define BRFC_EXPR_PROC_LIKE_HPP

#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace expr {
namespace proc {

struct like : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp operator()(const T&, const U&) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(const std::string& str, std::string pattern) const;

    sexp operator()(const sexp& args) const {
        return binary_dispatch(*this, args);
    }
};

} // namespace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_LIKE_HPP
