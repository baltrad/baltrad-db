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

#include <brfc/rdb/BindMap.hpp>

#include <utility>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace rdb {

BindMap::BindMap()
        : binds_() {

}

BindMap::BindMap(const BindMap& other)
        : binds_(other.binds_) {

}

BindMap&
BindMap::operator=(const BindMap& rhs) {
    if (this != &rhs) {
        binds_ = rhs.binds_;
    }
    return *this;
}

void
BindMap::add(const String& name, const Variant& value) {
    const map::value_type& pair = std::make_pair(name_to_placeholder(name),
                                                 value);
    if (not binds_.insert(pair).second)
        throw duplicate_entry(name.to_std_string());
}

bool
BindMap::has(const String& name) const {
    return binds_.find(name_to_placeholder(name)) != binds_.end();
}

const Variant&
BindMap::get(const String& name) const {
    map::const_iterator iter = binds_.find(name_to_placeholder(name));
    if (iter == binds_.end())
        throw lookup_error(name.to_std_string());
    return iter->second;
}

const Variant&
BindMap::get(const String& name, const Variant& default_) const {
    try {
        return get(name);
    } catch (const lookup_error&) {
        return default_;
    }
}

String
BindMap::name_to_placeholder(const String& name) const {
    String placeholder(name);
    if (not placeholder.starts_with(":"))
        placeholder.prepend(":");
    return placeholder;
}

bool
BindMap::remove(const String& name) {
    return binds_.erase(name_to_placeholder(name));
}

} // namespace rdb
} // namespace brfc
