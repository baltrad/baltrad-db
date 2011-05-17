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

#include <brfc/oh5/Oh5Group.hpp>

#include <list>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5NodeBackend.hpp>

namespace brfc {

Oh5Group::Oh5Group(const std::string& name)
        : Oh5Node(name) {

}

Oh5Group::Oh5Group(const Oh5Group& other)
        : Oh5Node(other) {

}

Oh5Group::~Oh5Group() {

}

Oh5Attribute*
Oh5Group::effective_attribute(const std::string& name) {
    const Oh5Group* self = const_cast<const Oh5Group*>(this);
    return const_cast<Oh5Attribute*>(self->effective_attribute(name));
}

const Oh5Attribute*
Oh5Group::effective_attribute(const std::string& name) const {
    const Oh5Attribute* child = attribute(name);
    if (child)
        return child;
    const Oh5Group* pgroup = parent<Oh5Group>();
    if (pgroup)
        return pgroup->effective_attribute(name);
    return 0;
}

Oh5Group&
Oh5Group::get_or_create_group(const std::string& pathstr) {
    if (boost::starts_with(pathstr, "/") and parent())
        throw value_error("path must not be absolute");
    
    std::list<std::string> path;
    boost::split(path, pathstr, boost::is_any_of("/"), boost::token_compress_on);

    Oh5Node* last = this;
    Oh5Node* node = 0;

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
        last = &last->backend().add(*last, new Oh5Group(*(iter++)));
    }

    return static_cast<Oh5Group&>(*last);
}

bool
Oh5Group::do_accepts_child(const Oh5Node& node) const {
    return true;
}

Oh5Node*
Oh5Group::do_clone() const {
    return new Oh5Group(*this);
}

} // namespace brfc
