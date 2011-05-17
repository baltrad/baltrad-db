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
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/FakeFile.hpp>

namespace brfc {
namespace oh5 {

struct oh5_extract_attr_test : public ::testing::Test {
    oh5_extract_attr_test()
            : f1()
            , proc() {
    }

    virtual void SetUp() {
        Node& what = f1.root().add(new Group("what"));
        what.add(new Attribute("source", Scalar("WMO:012345,CMT:foo")));

        Node& ds1 = f1.root().add(new Group("dataset1"));
        Node& ds1_where = ds1.add(new Group("where"));
        ds1_where.add(new Attribute("xsize", Scalar(1)));
        ds1_where.add(new Attribute("ysize", Scalar(2)));

        Node& ds2 = f1.root().add(new Group("dataset2"));
        Node& ds2_where = ds2.add(new Group("where"));
        ds2_where.add(new Attribute("xsize", Scalar(3)));
        ds2_where.add(new Attribute("ysize", Scalar(4)));

        Node& ds2_d1 = ds2.add(new Group("data1"));
        Node& ds2_d1_where = ds2_d1.add(new Group("where"));
        ds2_d1_where.add(new Attribute("xsize", Scalar(5)));
        ds2_d1_where.add(new Attribute("ysize", Scalar(6)));

        proc.file(&f1);
    }

    FakeFile f1;
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

} // namespace oh5
} // namespace brfc
