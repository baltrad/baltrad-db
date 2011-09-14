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

#include <brfc/oh5/MockOh5FileReader.hpp>

using ::testing::Ref;

namespace brfc {

TEST(oh5_Oh5FileReader_test, test_read) {
    Oh5File file;
    MockOh5FileReader reader;

    EXPECT_CALL(reader, do_read("/path", Ref(file)));

    reader.read("/path", file);
    EXPECT_EQ("/path", file.path());
}

} // namespace brfc
