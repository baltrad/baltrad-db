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

#include <brfc/oh5/SplitPath.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

namespace brfc {
namespace oh5 {

SplitPath::SplitPath(const String& path)
        : dataset_group_()
        , data_group_()
        , quality_group_()
        , attribute_group_()
        , attribute_()
        , dataset_() {

    split(path);
}

String
SplitPath::full_attribute_name() const {
    if (attribute_group_ != "") {
        return attribute_group_ + String("/") + attribute_;
    } else {
        return attribute_;
    }
}

String
SplitPath::group_path() const {
    StringList names;
    if (dataset_group_ != "")
        names.append(dataset_group_);
    if (data_group_ != "")
        names.append(data_group_);
    if (quality_group_ != "")
        names.append(quality_group_);
    if (attribute_group_ != "")
        names.append(attribute_group_);
    return String("/") + names.join("/");
}

namespace {

void set_once(String& holder, const String& value) {
    if (holder != "")
        throw value_error("duplicate set");
    holder = value;
}

} // namespace anonymous

void
SplitPath::split(const String& path) {
    // this is a very naive way to do it
    
    StringList names = path.split("/");
    BOOST_FOREACH(const String& name, names) {
        if (name.starts_with("dataset")) {
            set_once(dataset_group_, name);
        } else if (name == "data") {
            set_once(dataset_, name);
        } else if (name.starts_with("data")) {
            set_once(data_group_, name);
        } else if (name.starts_with("quality")) {
            set_once(quality_group_, name);
        } else if (name == "what" or name == "where" or name == "how") {
            set_once(attribute_group_, name);
        } else {
            set_once(attribute_, name);
        }
    }
}

} // namespace oh5
} // namespace brfc
