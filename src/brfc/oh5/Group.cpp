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

#include <brfc/oh5/Group.hpp>

#include <list>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {
namespace oh5 {

Group::Group(const std::string& name)
    : Node(name) {

}

Group::~Group() {

}

Attribute*
Group::attribute(const std::string& path) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->attribute(path));
}

const Attribute*
Group::attribute(const std::string& path) const {
    return dynamic_cast<const Attribute*>(child(path));
}

Attribute*
Group::effective_attribute(const std::string& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_cast<Attribute*>(self->effective_attribute(name));
}

const Attribute*
Group::effective_attribute(const std::string& name) const {
    const Attribute* child = attribute(name);
    if (child)
        return child;
    const Group* pgroup = parent<Group>();
    if (pgroup)
        return pgroup->effective_attribute(name);
    return 0;
}

Group*
Group::group(const std::string& path) {
    return dynamic_cast<Group*>(child(path));
}

const Group*
Group::group(const std::string& name) const {
    return dynamic_cast<const Group*>(child(name));
}

Group&
Group::get_or_create_group(const std::string& pathstr) {
    if (boost::starts_with(pathstr, "/") and not is_root())
        throw value_error("path must not be absolute");
    
    std::list<std::string> path;
    boost::split(path, pathstr, boost::is_any_of("/"), boost::token_compress_on);

    Group* last = this;
    Group* node = 0;

    std::list<std::string>::const_iterator iter = path.begin();

    // seek existing nodes
    while (iter != path.end()) {
        if (iter->empty())
            continue;
        node = last->group(*iter);
        if (not node)
            break;
        last = node;
        ++iter;
    }
    
    // create missing nodes
    while (iter != path.end()) {
        last = &last->create_group(*(iter++));
    }

    return *last;
}

bool
Group::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const RootGroup*>(&node))
        return false;
    return true;
}

} // namespace oh5
} // namespace brfc
