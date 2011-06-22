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

#include <brfc/oh5/Oh5File.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

Oh5Source
Oh5File::source() const {
    return do_source();
}

Oh5Source
Oh5File::do_source() const {
    const Oh5Attribute* attr = root().attribute("what/source");
    if (attr)
        return Oh5Source::from_string(attr->value().to_string());
    return Oh5Source();
}

namespace {

const Oh5Attribute&
get_attribute(const Oh5Node& node, const std::string& name) {
    const Oh5Attribute* attr = node.attribute(name);
    if (not attr)
        throw lookup_error("missing attribute: " + name);
    return *attr;
}

} // namespace anonymous

std::string
Oh5File::what_object() const {
    return get_attribute(root(), "what/object").value().string();
}

Date
Oh5File::what_date() const {
    return get_attribute(root(), "what/date").value().to_date();
}

Time
Oh5File::what_time() const {
    return get_attribute(root(), "what/time").value().to_time();
}

std::string
Oh5File::what_source() const {
    return get_attribute(root(), "what/source").value().string();
}

Oh5Node*
Oh5File::node(const std::string& path) {
    const Oh5File* self = const_cast<const Oh5File*>(this);
    return const_cast<Oh5Node*>(self->node(path));
}

const Oh5Node*
Oh5File::node(const std::string& path) const {
    if (path == "/")
        return &root();
    return root().child(path);
}

const Oh5Attribute*
Oh5File::attribute(const std::string& path) const {
    return dynamic_cast<const Oh5Attribute*>(node(path));
}

Oh5Attribute*
Oh5File::attribute(const std::string& path) {
    const Oh5File* self = const_cast<const Oh5File*>(this);
    return const_cast<Oh5Attribute*>(self->attribute(path));
}

Oh5Group*
Oh5File::group(const std::string& path) {
    const Oh5File* self = const_cast<const Oh5File*>(this);
    return const_cast<Oh5Group*>(self->group(path));
}

const Oh5Group*
Oh5File::group(const std::string& path) const {
    return dynamic_cast<const Oh5Group*>(node(path));
}

} // namepsace brfc
