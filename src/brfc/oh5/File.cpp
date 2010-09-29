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

#include <brfc/oh5/File.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/StringList.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

Source
File::source() const {
    const Attribute* attr =
        dynamic_cast<const Attribute*>(root().child("what/source"));
    if (attr)
        return Source::from_string(attr->value().to_string());
    return Source();
}

void
File::source(const Source& source) {
    Group& grp = root().get_or_create_child_group_by_name("what");
    Scalar srcval = Scalar(source.to_string());
    Attribute* attr = grp.child_attribute("source");
    if (attr)
        attr->value(srcval);
    else
        grp.create_attribute("source", srcval);
}

namespace {

const Attribute&
get_child_attribute(const Group& grp, const String& name) {
    const Attribute* attr = grp.child_attribute(name);
    if (not attr)
        throw lookup_error("missing attribute: " + name.to_utf8());
    return *attr;
}

} // namespace anonymous

String
File::what_object() const {
    return get_child_attribute(root(), "what/object").value().string();
}

Date
File::what_date() const {
    return get_child_attribute(root(), "what/date").value().to_date();
}

Time
File::what_time() const {
    return get_child_attribute(root(), "what/time").value().to_time();
}

String
File::what_source() const {
    return get_child_attribute(root(), "what/source").value().string();
}

String
File::name() const {
    return path().section("/", -1);
}

Group*
File::group(const String& path) {
    const File* self = const_cast<const File*>(this);
    return const_cast<Group*>(self->group(path));
}

const Group*
File::group(const String& path) const {
    if (path == "/")
        return &root();
    const Node* node = root().child(path);
    return dynamic_cast<const Group*>(node);
}

} // namespace oh5
} // namepsace brfc
