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

#include <map>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

namespace brfc {
namespace oh5 {

namespace {

typedef std::map<String, String> ElementMap;
    
ElementMap
parse_source(const String& source) {
    ElementMap map;
    StringList elems = source.split(",");
    try {
        BRFC_ASSERT(elems.size() != 0);
        BOOST_FOREACH(const String& elem, elems) {
            StringList kv = elem.split(":");
            BRFC_ASSERT(kv.size() == 2);
            BRFC_ASSERT(kv.at(0).length() > 0);
            BRFC_ASSERT(kv.at(1).length() > 0);
            map[kv.at(0)] = kv.at(1);
        }
    } catch (const assertion_error& e) {
        throw value_error("Invalid source string: " + source.to_std_string());
    }
    return map;
}

const String&
get_default(const ElementMap& map,
            const String& key,
            const String& default_) {
    ElementMap::const_iterator i = map.find(key);
    return i != map.end() ? i->second
                          : default_;
}

bool
contains(const ElementMap& map, const String& key) {
    return map.find(key) != map.end();
}


} // namespace anonymous

shared_ptr<Source>
Source::from_source_attribute(const String& source) {
    const ElementMap& elems = parse_source(source);
    if (contains(elems, "WMO") or
        contains(elems, "RAD") or
        contains(elems, "PLC")) {
        
        shared_ptr<SourceRadar> src = make_shared<SourceRadar>();
        src->wmo_code(get_default(elems, "WMO", "0").to_int());
        src->radar_site(get_default(elems, "RAD", ""));
        src->place(get_default(elems, "PLC", ""));
        return src;
    } else if (contains(elems, "CTY") or contains(elems, "ORG")) {
        shared_ptr<SourceCentre> src = make_shared<SourceCentre>();
        src->country_code(get_default(elems, "CTY", "0").to_int());
        src->originating_centre(get_default(elems, "ORG", "0").to_int());
        return src;
    } else {
        throw value_error("no fields in source to determine type");
    }
}

} // namespace oh5
} // namespace brfc
