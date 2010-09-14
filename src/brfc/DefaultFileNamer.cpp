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

#include <brfc/DefaultFileNamer.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {

DefaultFileNamer::DefaultFileNamer() {
}

String
DefaultFileNamer::do_name(const oh5::File& f) const {
    String name = String("Z_");
    name.append(f.what_object());
    if (f.source().has("CCCC")) {
        name.append("_C_");
        name.append(f.source().at("CCCC"));
    }
    name.append("_");
    name.append(f.what_date().to_string("yyyyMMdd"));
    name.append(f.what_time().to_string("hhmmss"));
    name.append("_");
    name.append(f.source().at("node"));
    name.append(".h5");

    return name;
}

} // namespace brfc
