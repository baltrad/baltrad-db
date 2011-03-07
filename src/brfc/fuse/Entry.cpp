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

#include <brfc/fuse/Entry.hpp>

#include <list>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/fuse/NoEntry.hpp>

namespace brfc {
namespace fuse {

Entry&
Entry::find(const char* path) {
    static NoEntry no_entry;

    std::list<std::string> tokens;
    std::string pathstr(path+1);
    if (pathstr.empty())
        return *this;

    boost::split(tokens, pathstr, boost::is_any_of("/"));
    
    Entry* entry = this;
    for (std::list<std::string>::const_iterator iter = tokens.begin();
         iter != tokens.end() and entry;
         ++iter) {
        entry = entry->child(*iter);
    }
    return entry ? *entry
                 : no_entry;
}

} // namespace fuse
} // namespace brfc
