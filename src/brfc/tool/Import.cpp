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

#include <brfc/tool/Import.hpp>

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/db/Database.hpp>

#include <brfc/oh5/hl/HlFile.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {

std::string
Import::do_description() const {
    return "import file(s) to the database";
}

void
Import::do_help(std::ostream& /*out*/) const {
    
}

void
Import::do_parse_args(const ArgVector& args) {
    if (args.empty())
        throw value_error("missing input files");

    args_ = args;
}

int
Import::do_execute(db::Database& db) {
    BOOST_FOREACH(const std::string& path, args_) {
        std::cout << "importing " << path << std::endl;;
        std::cout.flush();
        oh5::hl::HlFile file(path);
        db.store(file);
    }
    return 0;
}

} // namespace tool
} // namespace brfc
