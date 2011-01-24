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

#ifndef BRFC_TOOL_MOUNT_HPP
#define BRFC_TOOL_MOUNT_HPP

#include <sys/types.h>

#include <boost/program_options/options_description.hpp>

#include <brfc/tool/Command.hpp>

namespace brfc {
namespace tool {

class Mount : public Command {
  public:
    Mount();
  
  protected:
    virtual std::string do_description() const;

    virtual void do_help(std::ostream& out) const;

    virtual void do_parse_args(const ArgVector& vec);

    virtual int do_execute(db::Database& db);
  
  private:
    boost::program_options::options_description optdesc_;
    
    std::string mount_point_;
    bool debug_;
    bool foreground_;
    uid_t uid_;
    gid_t gid_;
};

} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_MOUNT_HPP
