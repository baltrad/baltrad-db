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

#include <brfc/oh5/SourceCentre.hpp>

#include <QtCore/QStringList>

namespace brfc {
namespace oh5 {

QString
SourceCentre::to_string() const {
    QStringList elems;
    if (originating_centre_ != 0)
        elems.append("ORG:" + QString::number(originating_centre_));
    if (country_code_ != 0)
        elems.append("CTY:" + QString::number(country_code_));
    return elems.join(",");
}

} // namespace oh5
} // namespace brfc
