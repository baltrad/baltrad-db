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

#ifndef BRFC_EXPR_PROC_ARITHMETIC_HPP
#define BRFC_EXPR_PROC_ARITHMETIC_HPP

#include <boost/mpl/and.hpp>
#include <boost/type_traits/is_arithmetic.hpp>

#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace expr {
namespace proc {

// + could additionally be defined for string + string, list + list
struct add : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp dispatch(const T&, const U&, boost::mpl::false_) const {
        BRFC_ASSERT(false);
        return sexp();
    }
    
    template<typename T, typename U>
    sexp dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return sexp(lhs + rhs);
    }

    template<typename T, typename U>
    sexp operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    sexp operator()(const sexp& args) const {
        return binary_dispatch(*this, args);
    }
};

struct sub : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp dispatch(const T&, const U&, boost::mpl::false_) const {
        BRFC_ASSERT(false);
        return sexp();
    }
    
    template<typename T, typename U>
    sexp dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return sexp(lhs - rhs);
    }

    template<typename T, typename U>
    sexp operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    sexp operator()(const sexp& args) const {
        return binary_dispatch(*this, args);
    }
};

struct mul : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp dispatch(const T&, const U&, boost::mpl::false_) const {
        BRFC_ASSERT(false);
        return sexp();
    }
    
    template<typename T, typename U>
    sexp dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return sexp(lhs * rhs);
    }

    template<typename T, typename U>
    sexp operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    sexp operator()(const sexp& args) const {
        return binary_dispatch(*this, args);
    }
};


struct div : public static_visitor<sexp> {
    template<typename T, typename U>
    sexp dispatch(const T&, const U&, boost::mpl::false_) const {
        BRFC_ASSERT(false);
        return sexp();
    }
    
    template<typename T, typename U>
    sexp dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return sexp(lhs / rhs);
    }

    template<typename T, typename U>
    sexp operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    sexp operator()(const sexp& args) const {
        return binary_dispatch(*this, args);
    }
};

} // namespace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_ARITHMETIC_HPP
