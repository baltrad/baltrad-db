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

#include <brfc/expr/Expression.hpp>

namespace brfc {

class Listcons {
  public:
    Listcons()
            : exp_() {
    }

    Listcons& symbol(const std::string& value) {
        exp_.push_back(Expression::symbol(value));
        return *this;
    }

    Listcons& string(const std::string& value) {
        exp_.push_back(Expression(value));
        return *this;
    }

    Listcons& int64(long long value) {
        exp_.push_back(Expression(value));
        return *this;
    }

    Listcons& double_(double value) {
        exp_.push_back(Expression(value));
        return *this;
    }

    Listcons& bool_(bool value) {
        exp_.push_back(Expression(value));
        return *this;
    }

    Listcons& date(int year, int month, int day) {
        exp_.push_back(Expression(Date(year, month, day)));
        return *this;
    }

    Listcons& date(const Date& date) {
        exp_.push_back(Expression(date));
        return *this;
    }

    Listcons& time(int hour, int minute, int second=0, int msec=0) {
        exp_.push_back(Expression(Time(hour, minute, second, msec)));
        return *this;
    }

    Listcons& time(const Time& time) {
        exp_.push_back(Expression(time));
        return *this;
    }

    Listcons& datetime(int year, int month, int day,
                       int hour, int minute, int second=0, int msec=0) {
        exp_.push_back(Expression(DateTime(year, month, day,
                                      hour, minute, second, msec)));
        return *this;
    }

    Listcons& datetime(const DateTime& dt) {
        exp_.push_back(Expression(dt));
        return *this;
    }

    Listcons& append(const Expression& value) {
        exp_.push_back(value);
        return *this;
    }

    Listcons& extend(const Expression& value) {
        exp_.extend(value);
        return *this;
    }

    Expression get() const { return exp_; }

  private:
    Expression exp_;
};

} // namespace brfc

#endif // BRFC_EXPR_LISTCONS_HPP
