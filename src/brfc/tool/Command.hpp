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

namespace brfc {

namespace db {
    class Database;
}

namespace tool {

/**
 * @brief ABC for bdbtool commands
 */
class Command {
  public:
    virtual ~Command() { };

    /**
     * @brief a short one-line description of the command
     */
    std::string description() const {
        return do_description();
    }
 
    /**
     * @brief put help to @c out
     */
    void help(std::ostream& out) const {
        do_help(out);
    }
        
    /**
     * @brief execute this command
     * @param fc filecatalog to run on
     * @param args arguments to the command
     * @return execution status
     */
    int execute(db::Database& db,
                const std::vector<std::string>& args) {
        return do_execute(db, args);
    }

  protected:
    virtual std::string do_description() const = 0;

    virtual void do_help(std::ostream& out) const = 0;

    virtual int do_execute(db::Database& db,
                           const std::vector<std::string>& args) = 0;
};

} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_COMMAND_HPP
