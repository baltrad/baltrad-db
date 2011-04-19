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
#ifndef BRFC_EXPR_LISTCONS_HPP
#define BRFC_EXPR_LISTCONS_HPP

#include <brfc/expr/sexp.hpp>

namespace brfc {
namespace expr {

class listcons {
  public:
    listcons()
            : sexp_() {
    }

    listcons& symbol(const std::string& value) {
        sexp_.push_back(sexp::symbol(value));
        return *this;
    }

    listcons& string(const std::string& value) {
        sexp_.push_back(sexp(value));
        return *this;
    }

    listcons& int64(long long value) {
        sexp_.push_back(sexp(value));
        return *this;
    }

    listcons& double_(double value) {
        sexp_.push_back(sexp(value));
        return *this;
    }

    listcons& bool_(bool value) {
        sexp_.push_back(sexp(value));
        return *this;
    }

    listcons& date(int year, int month, int day) {
        sexp_.push_back(sexp(Date(year, month, day)));
        return *this;
    }

    listcons& date(const Date& date) {
        sexp_.push_back(sexp(date));
        return *this;
    }

    listcons& time(int hour, int minute, int second=0, int msec=0) {
        sexp_.push_back(sexp(Time(hour, minute, second, msec)));
        return *this;
    }

    listcons& time(const Time& time) {
        sexp_.push_back(sexp(time));
        return *this;
    }

    listcons& datetime(int year, int month, int day,
                       int hour, int minute, int second=0, int msec=0) {
        sexp_.push_back(sexp(DateTime(year, month, day,
                                      hour, minute, second, msec)));
        return *this;
    }

    listcons& datetime(const DateTime& dt) {
        sexp_.push_back(sexp(dt));
        return *this;
    }

    listcons& append(const sexp& value) {
        sexp_.push_back(value);
        return *this;
    }

    sexp get() const { return sexp_; }

  private:
    sexp sexp_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_LISTCONS_HPP
