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

#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/StringList.hpp>

namespace brfc {
namespace oh5 {

String
SourceRadar::to_string() const {
    StringList elems;
    if (wmo_code_ != 0)
        elems.append("WMO:" + String::number(wmo_code_));
    if (radar_site_ != "")
        elems.append("RAD:" + radar_site_);
    if (place_ != "")
        elems.append("PLC:" + place_);
    return elems.join(",");
}

} // namespace oh5
} // namespace brfc
