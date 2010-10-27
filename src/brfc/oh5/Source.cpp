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

#include <brfc/oh5/Source.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

namespace brfc {
namespace oh5 {

Source
Source::from_string(const String& source) {
    Source src;
    StringList elems = source.split(",", String::SKIP_EMPTY_PARTS);
    try {
        BOOST_FOREACH(const String& elem, elems) {
            const StringList& kv = elem.split(":");
            BRFC_ASSERT(kv.size() == 2);
            BRFC_ASSERT(kv.at(0).length() > 0);
            if (kv.at(1).length() == 0)
                continue;
            BRFC_ASSERT(kv.at(1).length() > 0);
            src.add(kv.at(0), kv.at(1));
        }
    } catch (const assertion_error& e) {
        throw value_error("Invalid source string: " + source.to_std_string());
    }
    return src;
}

void
Source::add(const String& key, const String& value) {
    if (not map_.insert(std::make_pair(key, value)).second)
        throw duplicate_entry(key.to_std_string());
}

bool
Source::has(const String& key) const {
    return map_.find(key) != map_.end();
}

const String&
Source::get(const String& key) const {
    Map::const_iterator i = map_.find(key);
    if (i == map_.end())
        throw lookup_error(key.to_std_string());
    return i->second;
}

StringList
Source::keys() const {
    StringList keys;
    BOOST_FOREACH(const Map::value_type& kv, map_) {
        keys.push_back(kv.first);
    }
    return keys;
}

void
Source::remove(const String& key) {
    if (map_.erase(key) == 0)
        throw lookup_error(key.to_std_string());
}

void
Source::clear() {
    map_.clear();
}

bool
Source::empty() const {
    return map_.empty();
}

String
Source::to_string() const {
    StringList elms;
    BOOST_FOREACH(const Map::value_type& kv, map_) {
        elms.push_back(kv.first + ":" + kv.second);
    }
    return elms.join(",");
}

} // namespace oh5
} // namespace brfc
