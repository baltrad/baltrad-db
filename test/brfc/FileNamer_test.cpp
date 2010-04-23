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

#include <gtest/gtest.h>

#include <brfc/FileNamer.hpp>

#include "common.hpp"

namespace brfc {

TEST(FileNamer_test, with_marked_version) {
    QString original("filename_$VERSION$.ext");
    unsigned int version = 1;
    QString expected("filename_000001.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, with_dots) {
    QString original("file.name.ext");
    unsigned int version = 1;
    QString expected("file.name_000001.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, without_dots) {
    QString original("filename");
    unsigned int version = 1;
    QString expected("filename_000001");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, large_number) {
    QString original("filename_$VERSION$.ext");
    unsigned int version = 1234567;
    QString expected("filename_1234567.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

} // namespace brfc
