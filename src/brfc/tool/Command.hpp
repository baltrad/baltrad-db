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
    typedef std::vector<std::string> ArgVector;

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
     * @brief parse command-line arguments
     * @param args arguments to the command
     * @throw value_error if @c args can't be parsed
     *
     * will split the args using whitespace as delimiter
     */
    void parse_args(const std::string& args);
    
    /**
     * @brief parse command-line arguments
     * @param args arguments to the command
     * @throw value_error if @c args can't be parsed
     */

    void parse_args(const ArgVector& args) {
        return do_parse_args(args);
    }
        
    /**
     * @brief execute this command
     * @param fc filecatalog to run on
     * @return execution status
     */
    int execute(db::Database& db) {
        return do_execute(db);
    }

  protected:
    virtual std::string do_description() const = 0;

    virtual void do_help(std::ostream& out) const = 0;

    virtual void do_parse_args(const ArgVector& args) = 0;

    virtual int do_execute(db::Database& db) = 0;
    
};

} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_COMMAND_HPP
