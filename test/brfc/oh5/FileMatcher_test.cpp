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

#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Listcons.hpp>

#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/FakeFile.hpp>
#include <brfc/oh5/FileMatcher.hpp>

namespace brfc {
namespace oh5 {

struct oh5_FileMatcher_test : public ::testing::Test {
    oh5_FileMatcher_test()
            : f1()
            , matcher() {
    }

    virtual void SetUp() {
        Node& what = f1.root().add(new Group("what"));
        what.add(new Attribute("source", Scalar("WMO:012345")));
        what.add(new Attribute("date", Scalar("20111213")));
        what.add(new Attribute("time", Scalar("141516")));
        what.add(new Attribute("object", Scalar("PVOL")));

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
    }

    FakeFile f1;
    FileMatcher matcher;
    ExpressionFactory xpr;
};

TEST_F(oh5_FileMatcher_test, test_what_object_eq) {
    Expression e1 = xpr.eq(xpr.attribute("what/object"), xpr.string("PVOL"));
    Expression e2 = xpr.eq(xpr.attribute("what/object"), xpr.string("SCAN"));
    EXPECT_TRUE(matcher.match(f1, e1));
    EXPECT_FALSE(matcher.match(f1, e2));
}

TEST_F(oh5_FileMatcher_test, test_what_object_in) {
    Expression e1 = xpr.in(xpr.attribute("what/object"), Listcons().string("PVOL").string("CVOL").get());
    Expression e2 = xpr.in(xpr.attribute("what/object"), Expression());
    EXPECT_TRUE(matcher.match(f1, e1));
    EXPECT_FALSE(matcher.match(f1, e2));
}

TEST_F(oh5_FileMatcher_test, test_what_source_wmo_eq) {
    Expression e1 = xpr.eq(xpr.attribute("what/source:WMO"), xpr.string("012345"));
    Expression e2 = xpr.eq(xpr.attribute("what/source:WMO"), xpr.string("054321"));

    EXPECT_TRUE(matcher.match(f1, e1));
    EXPECT_FALSE(matcher.match(f1, e2));
}

TEST_F(oh5_FileMatcher_test, test_and) {
    Expression e1 = xpr.and_(
        xpr.lt(xpr.attribute("where/xsize"), xpr.int64_(2)),
        xpr.gt(xpr.attribute("where/ysize"), xpr.int64_(2))
    );

    Expression e2 = xpr.and_(
        xpr.lt(xpr.attribute("where/xsize"), xpr.int64_(1)),
        xpr.gt(xpr.attribute("where/ysize"), xpr.int64_(2))
    );

    EXPECT_TRUE(matcher.match(f1, e1));
    EXPECT_FALSE(matcher.match(f1, e2));
}

TEST_F(oh5_FileMatcher_test, test_or) {
    Expression e1 = xpr.or_(
        xpr.lt(xpr.attribute("where/xsize"), xpr.int64_(1)),
        xpr.gt(xpr.attribute("where/ysize"), xpr.int64_(2))
    );

    Expression e2 = xpr.or_(
        xpr.lt(xpr.attribute("where/xsize"), xpr.int64_(1)),
        xpr.gt(xpr.attribute("where/ysize"), xpr.int64_(6))
    );

    EXPECT_TRUE(matcher.match(f1, e1));
    EXPECT_FALSE(matcher.match(f1, e2));
}



} // namespace oh5
} // namespace brfc
