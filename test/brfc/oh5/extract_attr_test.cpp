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

#include <brfc/expr/Listcons.hpp>

#include <brfc/oh5/extract_attr.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>

namespace brfc {

struct oh5_extract_attr_test : public ::testing::Test {
    oh5_extract_attr_test()
            : metadata()
            , proc() {
    }

    virtual void SetUp() {
        Oh5Node& root = metadata.root();
        Oh5Node& what = root.add(new Oh5Group("what"));
        what.add(new Oh5Attribute("source", Oh5Scalar("WMO:012345,CMT:foo")));

        Oh5Node& ds1 = root.add(new Oh5Group("dataset1"));
        Oh5Node& ds1_where = ds1.add(new Oh5Group("where"));
        ds1_where.add(new Oh5Attribute("xsize", Oh5Scalar(1)));
        ds1_where.add(new Oh5Attribute("ysize", Oh5Scalar(2)));

        Oh5Node& ds2 = root.add(new Oh5Group("dataset2"));
        Oh5Node& ds2_where = ds2.add(new Oh5Group("where"));
        ds2_where.add(new Oh5Attribute("xsize", Oh5Scalar(3)));
        ds2_where.add(new Oh5Attribute("ysize", Oh5Scalar(4)));

        Oh5Node& ds2_d1 = ds2.add(new Oh5Group("data1"));
        Oh5Node& ds2_d1_where = ds2_d1.add(new Oh5Group("where"));
        ds2_d1_where.add(new Oh5Attribute("xsize", Oh5Scalar(5)));
        ds2_d1_where.add(new Oh5Attribute("ysize", Oh5Scalar(6)));

        proc.metadata(&metadata);
    }

    Oh5Metadata metadata;
    extract_attr proc;
};

TEST_F(oh5_extract_attr_test, test_many) {
    Expression in = Listcons().string("where/xsize").string("int64").get();

    Expression out = proc(in);
    EXPECT_EQ(3u, out.size());
    EXPECT_TRUE(out.contains(Expression(1)));
    EXPECT_TRUE(out.contains(Expression(3)));
    EXPECT_TRUE(out.contains(Expression(5)));
}

TEST_F(oh5_extract_attr_test, test_missing) {
    Expression in = Listcons().string("foo/bar").string("int64").get();

    Expression out = proc(in);
    EXPECT_EQ(0u, out.size());
}

TEST_F(oh5_extract_attr_test, test_source_part) {
    Expression in = Listcons().string("what/source:WMO").string("string").get();

    Expression out = proc(in);
    EXPECT_EQ(1u, out.size());
    EXPECT_TRUE(out.contains(Expression("012345")));
}

TEST_F(oh5_extract_attr_test, test_source_part_missing) {
    Expression in = Listcons().string("what/source:PLC").string("string").get();

    Expression out = proc(in);
    EXPECT_EQ(0u, out.size());
}

} // namespace brfc
