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

#include <brfc/oh5/File.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

Source
File::source() const {
    return do_source();
}

Source
File::do_source() const {
    const Attribute* attr = root().attribute("what/source");
    if (attr)
        return Source::from_string(attr->value().to_string());
    return Source();
}

namespace {

const Attribute&
get_attribute(const Node& node, const std::string& name) {
    const Attribute* attr = node.attribute(name);
    if (not attr)
        throw lookup_error("missing attribute: " + name);
    return *attr;
}

} // namespace anonymous

std::string
File::what_object() const {
    return get_attribute(root(), "what/object").value().string();
}

Date
File::what_date() const {
    return get_attribute(root(), "what/date").value().to_date();
}

Time
File::what_time() const {
    return get_attribute(root(), "what/time").value().to_time();
}

std::string
File::what_source() const {
    return get_attribute(root(), "what/source").value().string();
}

Node*
File::node(const std::string& path) {
    const File* self = const_cast<const File*>(this);
    return const_cast<Node*>(self->node(path));
}

const Node*
File::node(const std::string& path) const {
    if (path == "/")
        return &root();
    return root().child(path);
}

Group*
File::group(const std::string& path) {
    const File* self = const_cast<const File*>(this);
    return const_cast<Group*>(self->group(path));
}

const Group*
File::group(const std::string& path) const {
    return dynamic_cast<const Group*>(node(path));
}

} // namespace oh5
} // namepsace brfc
