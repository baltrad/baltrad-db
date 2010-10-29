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

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <brfc/FileCatalog.hpp>

#include <bdbtool/cmd/Import.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {
namespace cmd {

int
Import::do_execute(FileCatalog& fc,
                   const std::vector<std::string>& args) {
    BOOST_FOREACH(const std::string& path, args) {
        std::cout << "importing " << path << std::endl;;
        std::cout.flush();
        fc.store(path);
    }
    return 0;
}

} // namespace cmd
} // namespace tool
} // namespace brfc