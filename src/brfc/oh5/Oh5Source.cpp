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

#include <brfc/oh5/Oh5Source.hpp>

#include <list>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

namespace brfc {

Oh5Source
Oh5Source::from_string(const std::string& source) {
    Oh5Source src;
    std::list<std::string> elems;
    boost::split(elems, source, boost::is_any_of(","), boost::token_compress_on);
    try {
        BOOST_FOREACH(const std::string& elem, elems) {
            if (elem.empty())
                continue;
            BRFC_ASSERT(std::count(elem.begin(), elem.end(), ':') == 1);
            size_t colon_pos = elem.find(':');
            const std::string key = elem.substr(0, colon_pos);
            const std::string value = elem.substr(colon_pos + 1);
            BRFC_ASSERT(not key.empty());
            if (value.empty())
                continue;
            src.add(key, value);
        }
    } catch (const assertion_error& e) {
        throw value_error("Invalid source string: " + source);
    }
    return src;
}

void
Oh5Source::add(const std::string& key, const std::string& value) {
    if (not map_.insert(std::make_pair(key, value)).second)
        throw duplicate_entry("duplicate source key: " + key);
}

bool
Oh5Source::has(const std::string& key) const {
    return map_.find(key) != map_.end();
}

const std::string&
Oh5Source::get(const std::string& key) const {
    Map::const_iterator i = map_.find(key);
    if (i == map_.end())
        throw lookup_error("no source key: " + key);
    return i->second;
}

std::vector<std::string>
Oh5Source::keys() const {
    std::vector<std::string> keys;
    BOOST_FOREACH(const Map::value_type& kv, map_) {
        if (not boost::starts_with(kv.first, "_"))
           keys.push_back(kv.first);
    }
    return keys;
}

std::vector<std::string>
Oh5Source::all_keys() const {
    std::vector<std::string> keys;
    BOOST_FOREACH(const Map::value_type& kv, map_) {
        keys.push_back(kv.first);
    }
    return keys;
}

void
Oh5Source::remove(const std::string& key) {
    if (map_.erase(key) == 0)
        throw lookup_error("no source key: " + key);
}

void
Oh5Source::clear() {
    map_.clear();
}

bool
Oh5Source::empty() const {
    return map_.empty();
}

std::string
Oh5Source::to_string() const {
    std::list<std::string> elms;
    BOOST_FOREACH(const Map::value_type& kv, map_) {
        if (not boost::starts_with(kv.first, "_"))
            elms.push_back(kv.first + ":" + kv.second);
    }
    return boost::join(elms, ",");
}

} // namespace brfc
