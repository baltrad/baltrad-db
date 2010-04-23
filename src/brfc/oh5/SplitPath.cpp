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

#include <QtCore/QStringList>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

namespace brfc {
namespace oh5 {

SplitPath::SplitPath(const QString& path)
        : dataset_group_()
        , data_group_()
        , quality_group_()
        , attribute_group_()
        , attribute_()
        , dataset_() {

    split(path);
}

QString
SplitPath::full_attribute_name() const {
    if (attribute_group_ != "") {
        return attribute_group_ + QString("/") + attribute_;
    } else {
        return attribute_;
    }
}

QString
SplitPath::group_path() const {
    QStringList names;
    if (dataset_group_ != "")
        names.append(dataset_group_);
    if (data_group_ != "")
        names.append(data_group_);
    if (quality_group_ != "")
        names.append(quality_group_);
    if (attribute_group_ != "")
        names.append(attribute_group_);
    return QString("/") + names.join("/");
}

namespace {

void set_once(QString& holder, const QString& value) {
    if (holder != "")
        throw value_error("duplicate set");
    holder = value;
}

} // namespace anonymous

void
SplitPath::split(const QString& path) {
    // this is a very naive way to do it
    
    QStringList names = path.split("/");
    BOOST_FOREACH(const QString& name, names) {
        if (name.startsWith("dataset")) {
            set_once(dataset_group_, name);
        } else if (name == "data") {
            set_once(dataset_, name);
        } else if (name.startsWith("data")) {
            set_once(data_group_, name);
        } else if (name.startsWith("quality")) {
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
