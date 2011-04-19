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
#ifndef BRFC_EXPR_PROC_AGGREGATE_HPP
#define BRFC_EXPR_PROC_AGGREGATE_HPP

#include <brfc/expr/sexp.hpp>

#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace expr {
namespace proc {

struct date : public static_visitor<sexp> {
    template<typename T>
    sexp operator()(const T& value) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(const std::string& value) const {
        return sexp(Date::from_iso_string(value));
    }

    sexp operator()(const DateTime& value) const {
        return sexp(value.date());
    }

    sexp operator()(const Date& value) const {
        return sexp(value);
    }

    sexp operator()(const sexp& args) const {
        return unary_dispatch(*this, args);
    }
};

struct time : public static_visitor<sexp> {
    template<typename T>
    sexp operator()(const T& value) const {
        BRFC_ASSERT(false);
        return sexp();
    }
    
    sexp operator()(const std::string& value) const {
        return sexp(Time::from_iso_string(value));
    }

    sexp operator()(const DateTime& value) const {
        return sexp(value.time());
    }

    sexp operator()(const Time& value) const {
        return sexp(value);
    }

    sexp operator()(const sexp& args) const {
        return unary_dispatch(*this, args);
    }

};

struct datetime : public static_visitor<sexp> {
    template<typename T>
    sexp operator()(const T& value) const {
        BRFC_ASSERT(false);
        return sexp();
    }

    sexp operator()(const std::string& value) const {
        return sexp(DateTime::from_iso_string(value));
    }

    sexp operator()(const DateTime& value) const {
        return sexp(value);
    }

    sexp operator()(const sexp& args) const {
        return unary_dispatch(*this, args);
    }
};

} // namespace proc
} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_PROC_AGGREGATE_HPP
