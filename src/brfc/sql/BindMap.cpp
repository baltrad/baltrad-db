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

#include <brfc/sql/BindMap.hpp>

#include <utility>

#include <boost/algorithm/string/predicate.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace sql {

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
BindMap::add(const std::string& name, const Variant& value) {
    const map::value_type& pair = std::make_pair(name_to_placeholder(name),
                                                 value);
    if (not binds_.insert(pair).second)
        throw duplicate_entry("duplicate BindMap key: " + name);
}

void
BindMap::set(const std::string& name, const Variant& value) {
    const std::string& key = name_to_placeholder(name);
    map::iterator iter = binds_.find(key);
    if (iter == binds_.end())
        throw lookup_error("no BindMap key: " + name);
    binds_.erase(iter);
    binds_.insert(std::make_pair(key, value));
}

bool
BindMap::has(const std::string& name) const {
    return binds_.find(name_to_placeholder(name)) != binds_.end();
}

const Variant&
BindMap::get(const std::string& name) const {
    map::const_iterator iter = binds_.find(name_to_placeholder(name));
    if (iter == binds_.end())
        throw lookup_error("no BindMap key: " + name);
    return iter->second;
}

const Variant&
BindMap::get(const std::string& name, const Variant& default_) const {
    try {
        return get(name);
    } catch (const lookup_error&) {
        return default_;
    }
}

std::string
BindMap::name_to_placeholder(const std::string& name) const {
    std::string placeholder(name);
    if (not boost::starts_with(placeholder, ":"))
        placeholder = ":" + placeholder;
    return placeholder;
}

bool
BindMap::remove(const std::string& name) {
    return binds_.erase(name_to_placeholder(name));
}

} // namespace sql
} // namespace brfc
