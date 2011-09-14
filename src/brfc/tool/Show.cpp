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

#include <brfc/tool/Show.hpp>

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {

std::string
Show::do_description() const {
    return "show file details";
}

void
Show::do_help(std::ostream& /*out*/) const {
    
}

void
Show::do_parse_args(const ArgVector& args) {
    if (args.empty())
        throw std::invalid_argument("missing input uuids");

    args_ = args;
}

int
Show::do_execute(Database& db) {
    std::auto_ptr<FileEntry> entry;
    BOOST_FOREACH(const std::string& uuid, args_) {
        try {
            entry.reset(db.entry_by_uuid(uuid));
        } catch (const lookup_error&) {
            std::cout << uuid << " not found" << std::endl;
            continue;
        }
        const Oh5Metadata& meta = entry->metadata();
        std::cout << uuid << " "
                  << meta.what_date().to_iso_string(true) << "T"
                  << meta.what_time().to_iso_string(true) << " "
                  << meta.what_object() << " "
                  << entry->source().get("_name")
                  << std::endl;
        std::cout.flush();
    }
    return 0;
}

} // namespace tool
} // namespace brfc
