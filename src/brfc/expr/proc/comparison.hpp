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

#ifndef BRFC_EXPR_PROC_COMPARISON_HPP
#define BRFC_EXPR_PROC_COMPARISON_HPP

#include <brfc/expr/proc/dispatch.hpp>
#include <functional>

namespace brfc {
namespace proc {

struct eq : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::equal_to<Expression>(), args));
    }
};

struct ne : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::not_equal_to<Expression>(), args));
    }
};

struct lt : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::less<Expression>(), args));
    }
};

struct le : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::less_equal<Expression>(), args));
    }
};

struct gt : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::greater<Expression>(), args));
    }
};

struct ge : public static_visitor<Expression> {
    Expression operator()(const Expression& args) const {
        return Expression(binary_call(std::greater_equal<Expression>(), args));
    }
};

} // namespace proc
} // namespace brfc

#endif // BRFC_EXPR_PROC_COMPARISON_HPP
