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
#ifndef BRFC_TOOL_MOCK_COMMAND_HPP
#define BRFC_TOOL_MOCK_COMMAND_HPP

#include <gmock/gmock.h>

#include <brfc/db/Database.hpp>

#include <brfc/tool/Command.hpp>

namespace brfc {
namespace tool {

class MockCommand : public Command {
  public:
    MOCK_CONST_METHOD0(do_description,
        std::string());
    MOCK_CONST_METHOD1(do_help,
        void(std::ostream&));
    MOCK_METHOD1(do_parse_args,
        void(const ArgVector&));
    MOCK_METHOD1(do_execute,
        int(db::Database&));
};

} // namespace tool
} // namespace brfc

#endif // BRFC_TOOL_MOCK_COMMAND_HPP
