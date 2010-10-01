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

#include <brfc/TimeDelta.hpp>

namespace brfc {

TimeDelta&
TimeDelta::add_msecs(int msecs) {
    msecs_ += msecs;
    if (msecs_ < 0) {
        int negdays = (MSECS_IN_DAY - msecs_) / MSECS_IN_DAY;
        days_ -= negdays;
        msecs_ = msecs_ + negdays * MSECS_IN_DAY;
    } else {
        days_ += msecs / MSECS_IN_DAY;
        msecs_ = msecs_ % MSECS_IN_DAY;
    }
    return *this;
}

} // namespace brfc
