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

#include <brfc/oh5/Oh5Metadata.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

Oh5Metadata::Oh5Metadata()
        : nodes_() {

}

Oh5Metadata::~Oh5Metadata() {

}

const Oh5Node&
Oh5Metadata::root() const {
    return nodes_.root();
}

Oh5Node&
Oh5Metadata::root() {
    return nodes_.root();
}

const Oh5Node*
Oh5Metadata::node(const std::string& path) const {
    if (path == "/")
        return &root();
    return root().child(path);
}

Oh5Node*
Oh5Metadata::node(const std::string& path) {
    const Oh5Metadata* self = const_cast<const Oh5Metadata*>(this);
    return const_cast<Oh5Node*>(self->node(path));
}

const Oh5Attribute*
Oh5Metadata::attribute(const std::string& path) const {
    return dynamic_cast<const Oh5Attribute*>(node(path));
}

Oh5Attribute*
Oh5Metadata::attribute(const std::string& path) {
    const Oh5Metadata* self = const_cast<const Oh5Metadata*>(this);
    return const_cast<Oh5Attribute*>(self->attribute(path));
}

const Oh5Group*
Oh5Metadata::group(const std::string& path) const {
    return dynamic_cast<const Oh5Group*>(node(path));
}

Oh5Group*
Oh5Metadata::group(const std::string& path) {
    const Oh5Metadata* self = const_cast<const Oh5Metadata*>(this);
    return const_cast<Oh5Group*>(self->group(path));
}

Oh5Source
Oh5Metadata::source() const {
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

Oh5Group&
get_or_create_group(Oh5Node& node, const std::string& name) {
    Oh5Group* group = node.group(name);
    if (not group)
        return static_cast<Oh5Group&>(node.add(new Oh5Group(name)));
    return *group;
}

Oh5Attribute&
get_or_create_attribute(Oh5Node& node, const std::string& name) {
    Oh5Attribute* attr = node.attribute(name);
    if (not attr)
        return static_cast<Oh5Attribute&>(
            node.add(new Oh5Attribute(name, Oh5Scalar("")))
        );
    return *attr;
}

} // namespace anonymous

std::string
Oh5Metadata::what_object() const {
    return get_attribute(root(), "what/object").value().string();
}

void
Oh5Metadata::what_object(const std::string& value) {
    Oh5Node& what = get_or_create_group(root(), "what");
    Oh5Attribute& attr = get_or_create_attribute(what, "object");
    attr.value(Oh5Scalar(value));
}

Date
Oh5Metadata::what_date() const {
    return get_attribute(root(), "what/date").value().to_date();
}

void
Oh5Metadata::what_date(const Date& value) {
    Oh5Node& what = get_or_create_group(root(), "what");
    Oh5Attribute& attr = get_or_create_attribute(what, "date");
    attr.value(Oh5Scalar(value.to_iso_string()));  
}

Time
Oh5Metadata::what_time() const {
    return get_attribute(root(), "what/time").value().to_time();
}

void
Oh5Metadata::what_time(const Time& value) {
    Oh5Node& what = get_or_create_group(root(), "what");
    Oh5Attribute& attr = get_or_create_attribute(what, "time");
    attr.value(Oh5Scalar(value.to_iso_string()));  
}

std::string
Oh5Metadata::what_source() const {
    return get_attribute(root(), "what/source").value().string();
}

void
Oh5Metadata::what_source(const std::string& value) {
    Oh5Node& what = get_or_create_group(root(), "what");
    Oh5Attribute& attr = get_or_create_attribute(what, "source");
    attr.value(Oh5Scalar(value));
}

} // namepsace brfc
