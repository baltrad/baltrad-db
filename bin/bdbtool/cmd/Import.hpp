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

#ifndef BRFC_TOOL_CMD_IMPORT_HPP
#define BRFC_TOOL_CMD_IMPORT_HPP

#include <bdbtool/Command.hpp>

namespace brfc {
namespace tool {
namespace cmd {

class Import : public Command {
  protected:
    virtual std::string do_description() const;

    virtual void do_help(std::ostream& out) const;

    virtual int do_execute(db::Database& db,
                           const std::vector<std::string>& args);
};

} // namespace cmd
} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_CMD_IMPORT_HPP
