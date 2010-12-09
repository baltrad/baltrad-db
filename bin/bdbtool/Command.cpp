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

#include <brfc/buildconfig.h>

#include <bdbtool/Command.hpp>
#include <bdbtool/cmd/Benchmark.hpp>
#include <bdbtool/cmd/Import.hpp>

#ifdef BDB_BUILD_BDBFS
    #include <bdbtool/cmd/Mount.hpp>
#endif // BDB_BUILD_BDBFS

namespace brfc {
namespace tool {

shared_ptr<Command>
Command::by_name(const std::string& name) {
    shared_ptr<Command> c;
    if (name == "import") {
        c.reset(new cmd::Import());
    } else if (name == "benchmark") {
        c.reset(new cmd::Benchmark());
#ifdef BDB_BUILD_BDBFS
    } else if (name == "mount") {
        c.reset(new cmd::Mount());
#endif // BDB_BUILD_BDBFS
    }
    return c;
}

} // namespace tool
} // namespace brfc
