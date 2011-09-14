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

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

struct oh5_Oh5File_test : public testing::Test {
    oh5_Oh5File_test()
            : file() {
    }
    
    Oh5File file;
};

TEST_F(oh5_Oh5File_test, test_name) {
    file.path("");
    EXPECT_EQ(file.name(), "");

    file.path("/path/to/filename");
    EXPECT_EQ(file.name(), "filename");

    file.path("filename2");
    EXPECT_EQ(file.name(), "filename2");
}

TEST_F(oh5_Oh5File_test, test_open_nx_file) {
    EXPECT_THROW(Oh5File("/path/to/nxfile"), fs_error);
}

TEST_F(oh5_Oh5File_test, test_load_from_filesystem) {
    Oh5Scalar t_12_05_01(Time(12, 5, 1));
    Oh5Scalar d_2000_01_02(Date(2000, 1, 2));

    Oh5Node& froot = file.metadata().root(); 
    froot.add(new Oh5Attribute("date", d_2000_01_02));
    froot.add(new Oh5Attribute("time", t_12_05_01));
    Oh5Node& what = froot.add(new Oh5Group("what"));
    what.add(new Oh5Attribute("date", d_2000_01_02));
    Oh5Node& data1 = froot.add(new Oh5Group("data1"));
    data1.add(new Oh5DataSet("data"));
    
    NamedTemporaryFile tempfile;
    Oh5HlFileWriter writer;
    writer.write(file, tempfile.path());

    Oh5File g(tempfile.path());
    EXPECT_EQ(g.path(), tempfile.path());
    const Oh5Node& groot = g.metadata().root();
    EXPECT_EQ((size_t)4, groot.children().size());
    EXPECT_TRUE(groot.has_child("date"));
    EXPECT_TRUE(groot.has_child("time"));
    EXPECT_TRUE(groot.has_child("what"));
    ASSERT_TRUE(groot.attribute("date"));
    ASSERT_TRUE(groot.attribute("time"));
    ASSERT_TRUE(groot.group("/what"));
    ASSERT_TRUE(groot.attribute("/what/date"));
    EXPECT_EQ("20000102", groot.attribute("date")->value().string());
    EXPECT_EQ("120501", groot.attribute("time")->value().string());
    EXPECT_EQ("20000102", groot.attribute("what/date")->value().string());
    EXPECT_TRUE(groot.has_child("data1/data"));
    EXPECT_TRUE(dynamic_cast<const Oh5DataSet*>(groot.child("data1/data")) != 0);
}

} // namespace brfc
