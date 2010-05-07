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

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

const String&
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
StringList::contains(const String& str) const {
    BOOST_FOREACH(const String& entry, list_) {
        if (entry == str)
            return true;
    }
    return false;
}

String
StringList::join(const String& sep) const {
    String concat;
    BOOST_FOREACH(const String& entry, list_) {
        concat += entry;
        concat += sep;
    }
    
    if (list_.size() > 0)
        concat.remove(concat.length() - sep.length(), sep.length());
    return concat;
}

} // namespace brfc
