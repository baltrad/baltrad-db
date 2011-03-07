/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <gtest/gtest.h>

#include <brfc/CacheDirStorage.hpp>

#include <brfc/tool/MockCommand.hpp>

namespace brfc {
namespace tool {

class tool_Command_test : public ::testing::Test {
  public:
    tool_Command_test()
            : cmd() {
    }

    MockCommand cmd;
};

TEST_F(tool_Command_test, test_parse_args_str) {
    std::vector<std::string> expected;
    expected.push_back("--arg1");
    expected.push_back("val1");
    expected.push_back("-o");

    EXPECT_CALL(cmd, do_parse_args(expected));

    cmd.parse_args("--arg1 val1 -o");
}

TEST_F(tool_Command_test, test_parse_args_str_extra_whitespace) {
    std::vector<std::string> expected;
    expected.push_back("--arg1");
    expected.push_back("val1");
    expected.push_back("-o");

    EXPECT_CALL(cmd, do_parse_args(expected));

    cmd.parse_args("--arg1  val1\t \t-o");
}

} // namespace tool
} // namespace brfc
