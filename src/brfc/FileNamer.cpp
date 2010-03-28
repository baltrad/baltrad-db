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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/FileNamer.hpp>

namespace brfc {

QString
FileNamer::inject_version(const QString& filename,
                          unsigned int version) {
    QString vstring = QString::number(version).rightJustified(6, '0');
    QString newname(filename);
    if (filename.contains("$VERSION$")) {
        newname.replace("$VERSION$", vstring);
    } else if (filename.contains(".")) {
        int last_dot_pos = newname.lastIndexOf('.');
        newname.insert(last_dot_pos, "_" + vstring);
    } else {
        newname.append("_" + vstring);
    }
    return newname;
}

} // namespace brfc
