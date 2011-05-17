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

#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace proc {

// should we also support nil (i.e empty list)?

struct and_ : public static_visitor<Expression> {
    template<typename T, typename U>
    Expression operator()(const T&, const U&) const {
        BRFC_ASSERT(false);
        return Expression();
    }

    Expression operator()(bool lhs, bool rhs) const {
        return Expression(lhs and rhs);
    }

    Expression operator()(const Expression& args) const {
        return binary_dispatch(*this, args);
    }
};

struct or_ : public static_visitor<Expression> {
    template<typename T, typename U>
    Expression operator()(const T&, const U&) const {
        BRFC_ASSERT(false);
        return Expression();
    }

    Expression operator()(bool lhs, bool rhs) const {
        return Expression(lhs or rhs);
    }

    Expression operator()(const Expression& args) const {
        return binary_dispatch(*this, args);
    }
};

struct not_ : public static_visitor<Expression> {
    template<typename T>
    Expression operator()(const T&) const {
        BRFC_ASSERT(false);
        return Expression();
    }

    Expression operator()(bool val) const {
        return Expression(not val);
    }

    Expression operator()(const Expression& args) const {
        return unary_dispatch(*this, args);
    }
};

} // namespace proc
} // namespace brfc

#endif // BRFC_EXPR_PROC_LOGIC_HPP
