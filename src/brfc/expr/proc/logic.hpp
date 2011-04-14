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

#ifndef BRFC_EXPR_PROC_LOGIC_HPP
#define BRFC_EXPR_PROC_LOGIC_HPP

#include <brfc/assert.hpp>
#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {
namespace proc {

// should we also support nil (i.e empty list)?

struct and_ : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp operator()(const T&, const U&) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(bool lhs, bool rhs) const {
        return sexp(lhs and rhs);
    }
};

struct or_ : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp operator()(const T&, const U&) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(bool lhs, bool rhs) const {
        return sexp(lhs or rhs);
    }
};

struct not_ : public static_visitor<sexp> {
    template<typename T>
    sexp operator()(const T&) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(bool val) const {
        return sexp(not val);
    }
};

} // namespace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_LOGIC_HPP
