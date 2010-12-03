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

#include <brfc/StringList.hpp>

#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/xpressive/xpressive_static.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

namespace {

bool is_str_empty(const std::string& str) {
    return str == "";
}

}

StringList
StringList::split(const std::string& str,
                  const std::string& sep,
                  SplitPolicy pol) {
    if (sep == "")
        throw value_error("empty separator");
    std::list<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of(sep));
    std::list<std::string>::iterator end = tokens.end();
    if (pol == SKIP_EMPTY_PARTS)
        end = std::remove_if(tokens.begin(), end, is_str_empty);
    return StringList(tokens.begin(), end);
}

const std::string&
StringList::at(int i) const {
    const_iterator iter = begin();
    while (i > 0) {
        iter++;
        i--;
    }
    if (iter == end())
        throw value_error("index out of bounds");
    return *iter;
}

bool
StringList::contains(const std::string& str) const {
    BOOST_FOREACH(const std::string& entry, list_) {
        if (entry == str)
            return true;
    }
    return false;
}

std::string
StringList::join(const std::string& sep) const {
    return boost::join(list_, sep);
}

} // namespace brfc
