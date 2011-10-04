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

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>
#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

DefaultFileNamer::DefaultFileNamer() {

}

namespace {

std::string
name_from_metadata(const Oh5Metadata& meta, const Oh5Source& source) {
    std::string name;
    name.append(boost::to_lower_copy(meta.what_object()));
    name.append("_");
    if (source.has("_name")) {
        name.append(boost::to_lower_copy(source.get("_name")));
    } else if (source.has("NOD")) {
        name.append(boost::to_lower_copy(source.get("NOD")));
    } else {
        name.append("unknown");
    }
    name.append("_");
    name.append(meta.what_date().to_iso_string());
    name.append("T");
    name.append(meta.what_time().to_iso_string());
    name.append("Z");
    return name;
}

std::string
name_from_metadata(const Oh5Metadata& meta) {
    return name_from_metadata(meta, meta.source());
}

} // namespace anonymous

std::string
DefaultFileNamer::do_name(const Oh5File& file) const {
    std::string name = name_from_metadata(file.metadata());
    name.append(".h5");
    return name;
}

std::string
DefaultFileNamer::do_name(const FileEntry& entry) const {
    std::string name = name_from_metadata(entry.metadata(), entry.source());
    name.append("_");
    const std::string& uuid = entry.uuid();
    name.append(uuid.substr(0, uuid.find("-")));
    name.append(".h5");
    return name;
}

} // namespace brfc
