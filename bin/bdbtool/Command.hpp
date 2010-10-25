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

#ifndef BRFC_TOOL_COMMAND_HPP
#define BRFC_TOOL_COMMAND_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include <brfc/smart_ptr.hpp>

namespace brfc {

class FileCatalog;

namespace tool {

class Command {
  public:
    static shared_ptr<Command> by_name(const std::string& name);

    virtual ~Command() { };

    void help(std::ostream& out) const {
        return do_help(out);
    }

    int execute(FileCatalog& fc,
                const std::vector<std::string>& args) {
        return do_execute(fc, args);
    }

  protected:
    virtual int do_execute(FileCatalog& fc,
                           const std::vector<std::string>& args) = 0;
    
    virtual void do_help(std::ostream& /*out*/) const {
        // no-op
    }
};

} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_COMMAND_HPP
