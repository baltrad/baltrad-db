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
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/hl/HlFile.hpp>
#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>
#include <brfc/util/NamedTemporaryFile.hpp>

#include <brfc/test_common.hpp>

namespace brfc {

struct oh5_hl_HlFile_test : public testing::Test {
    oh5_hl_HlFile_test() {

    }

    HlFile file;
};

TEST_F(oh5_hl_HlFile_test, test_open_nx_file) {
    EXPECT_THROW(HlFile("/path/to/nxfile"), fs_error);
}

TEST_F(oh5_hl_HlFile_test, test_load_from_filesystem) {
    Oh5Scalar t_12_05_01(Time(12, 5, 1));
    Oh5Scalar d_2000_01_02(Date(2000, 1, 2));

    file.root().add(new Oh5Attribute("date", d_2000_01_02));
    file.root().add(new Oh5Attribute("time", t_12_05_01));
    Oh5Node& what = file.root().add(new Oh5Group("what"));
    what.add(new Oh5Attribute("date", d_2000_01_02));
    Oh5Node& data1 = file.root().add(new Oh5Group("data1"));
    data1.add(new Oh5DataSet("data"));
    
    NamedTemporaryFile tempfile;
    Oh5HlFileWriter writer;
    writer.write(file, tempfile.path());

    HlFile g(tempfile.path());
    EXPECT_EQ(g.path(), tempfile.path());
    Oh5Node& root = g.root();
    EXPECT_EQ((size_t)4, root.children().size());
    EXPECT_TRUE(root.has_child("date"));
    EXPECT_TRUE(root.has_child("time"));
    EXPECT_TRUE(root.has_child("what"));
    ASSERT_TRUE(root.attribute("date"));
    ASSERT_TRUE(root.attribute("time"));
    ASSERT_TRUE(root.group("/what"));
    ASSERT_TRUE(root.attribute("/what/date"));
    EXPECT_EQ("20000102", root.attribute("date")->value().string());
    EXPECT_EQ("120501", root.attribute("time")->value().string());
    EXPECT_EQ("20000102", root.attribute("what/date")->value().string());
    EXPECT_TRUE(root.has_child("data1/data"));
    EXPECT_TRUE(dynamic_cast<Oh5DataSet*>(root.child("data1/data")) != 0);
}

} // namespace brfc
