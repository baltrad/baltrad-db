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

#ifndef BRFC_TIME_DELTA_HPP
#define BRFC_TIME_DELTA_HPP

namespace brfc {

enum dt_const {
    HOURS_IN_DAY = 24,
    MINS_IN_HOUR = 60,
    SECS_IN_MIN = 60,
    MSECS_IN_SEC = 1000,
    MSECS_IN_MIN = 60000,
    MSECS_IN_HOUR = 3600000,
    MSECS_IN_DAY = 86400000
};

class TimeDelta {
  public:
    explicit TimeDelta(int days=0, int msecs=0)
            : days_(days)
            , msecs_(msecs) {
    }
    
    int days() const { return days_; }
    int msecs() const { return msecs_; }

    TimeDelta& days(int days) { days_ = days; return *this; }
    TimeDelta& msecs(int msecs) { msecs_ = msecs; return *this; }
    
    TimeDelta& add_days(int days) {
        days_ += days;
        return *this;
    }

    TimeDelta& add_hours(int hours) {
        return add_msecs(hours * MSECS_IN_HOUR);
    } 

    TimeDelta& add_minutes(int minutes) {
        return add_msecs(minutes * MSECS_IN_MIN);
    }

    TimeDelta& add_seconds(int seconds) {
        return add_msecs(seconds * MSECS_IN_SEC);
    }

    TimeDelta& add_msecs(int msecs);

    /**
     * @name comparison
     * @{
     */
    bool operator==(const TimeDelta& rhs) const {
        return msecs_ == rhs.msecs_ and days_ == rhs.days_;
    }

    bool operator!=(const TimeDelta& rhs) const {
        return not (*this == rhs);
    }

    bool operator<(const TimeDelta& rhs) const {
        return days_ < rhs.days_ and msecs_ < rhs.msecs_;
    }
    ///@}

  private:
    int days_;
    int msecs_;
};

} // namespace brfc

#endif // BRFC_TIME_DELTA_HPP
