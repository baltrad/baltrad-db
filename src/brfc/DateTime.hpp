/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_DATE_TIME_HPP
#define BRFC_DATE_TIME_HPP

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

namespace brfc {

class DateTime {
  public:
    explicit DateTime(int year=1, int month=1, int day=1,
                      int hour=0, int minute=0, int second=0, int msec=0)
            : date_(year, month, day)
            , time_(hour, minute, second, msec) {
    }

    DateTime(const Date& date, const Time& time)
            : date_(date)
            , time_(time) {
    }

    explicit DateTime(const Date& date)
            : date_(date)
            , time_(0, 0, 0, 0) {
    }

    explicit DateTime(const Time& time)
            : date_(1, 1, 1)
            , time_(time) {
    }

    const Date& date() const { return date_; }
    Date& date() { return date_; }

    const Time& time() const { return time_; }
    Time& time() { return time_; }

  private:
    Date date_;
    Time time_;
};

} // namespace brfc

#endif // BRFC_DATE_TIME_HPP
