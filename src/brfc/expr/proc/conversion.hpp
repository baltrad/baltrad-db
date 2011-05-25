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

#include <brfc/expr/proc/dispatch.hpp>

namespace brfc {
namespace proc {

struct date : public static_visitor<Expression> {
    template<typename T>
    Expression operator()(const T& value) const {
        BRFC_ASSERT(false);
        return Expression();
    }

    Expression operator()(const std::string& value) const {
        return Expression(Date::from_iso_string(value));
    }

    Expression operator()(const DateTime& value) const {
        return Expression(value.date());
    }

    Expression operator()(const Date& value) const {
        return Expression(value);
    }

    Expression operator()(const Expression& args) const {
        return unary_dispatch(*this, args);
    }
};

struct time : public static_visitor<Expression> {
    template<typename T>
    Expression operator()(const T& value) const {
        BRFC_ASSERT(false);
        return Expression();
    }
    
    Expression operator()(const std::string& value) const {
        return Expression(Time::from_iso_string(value));
    }

    Expression operator()(const DateTime& value) const {
        return Expression(value.time());
    }

    Expression operator()(const Time& value) const {
        return Expression(value);
    }

    Expression operator()(const Expression& args) const {
        return unary_dispatch(*this, args);
    }

};

struct datetime : public static_visitor<Expression> {
    template<typename T>
    Expression operator()(const T& value) const {
        BRFC_ASSERT(false);
        return Expression();
    }

    Expression operator()(const std::string& value) const {
        return Expression(DateTime::from_iso_string(value));
    }

    Expression operator()(const DateTime& value) const {
        return Expression(value);
    }

    Expression operator()(const Expression& args) const {
        return unary_dispatch(*this, args);
    }
};

} // namespace proc
} // namespace brfc

#endif // BRFC_EXPR_PROC_AGGREGATE_HPP
