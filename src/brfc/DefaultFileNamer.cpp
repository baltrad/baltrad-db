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

#include <brfc/DefaultFileNamer.hpp>

#include <boost/algorithm/string/case_conv.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {

DefaultFileNamer::DefaultFileNamer() {

}

std::string
DefaultFileNamer::do_name(const oh5::File& file) const {
    std::string name;
    name.append(boost::to_lower_copy(file.what_object()));
    name.append("_");
    if (file.source().has("_name")) {
        name.append(boost::to_lower_copy(file.source().get("_name")));
    } else {
        name.append("unknown");
    }
    name.append("_");
    name.append(file.what_date().to_iso_string());
    name.append("T");
    name.append(file.what_time().to_iso_string());
    name.append("Z");
    name.append(".h5");
    return name;
}

std::string
DefaultFileNamer::do_name(const db::FileEntry& entry) const {
    const oh5::File& file = static_cast<const oh5::File&>(entry);
    std::string name = this->name(file);
    const std::string& uuid = entry.uuid();
    const std::string& version = "_" + uuid.substr(0, uuid.find("-"));
    name.insert(name.rfind("."), version);
    return name;
}

} // namespace brfc
