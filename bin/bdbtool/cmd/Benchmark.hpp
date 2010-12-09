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

#ifndef BRFC_TOOL_CMD_BENCHMARK_HPP
#define BRFC_TOOL_CMD_BENCHMARK_HPP

#include <bdbtool/Command.hpp>

#include <boost/program_options/options_description.hpp>

namespace brfc {
namespace tool {
namespace cmd {

class Benchmark : public Command {
  public:
    Benchmark();

  protected:
    virtual std::string do_description() const;

    virtual int do_execute(FileCatalog& fc,
                           const std::vector<std::string>& args);
    virtual void do_help(std::ostream& out) const;
  
  private:
    int iterations_;
    boost::program_options::options_description optdesc_;
};

} // namespace cmd
} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_CMD_BENCHMARK_HPP
