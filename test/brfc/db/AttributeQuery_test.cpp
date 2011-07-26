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
#include <brfc/db/AttributeQuery.hpp>

namespace brfc {

class db_AttributeQuery_test : public ::testing::Test {
  public:
    db_AttributeQuery_test()
        : query() {
    }
    
    AttributeQuery query;
};

TEST_F(db_AttributeQuery_test, test_copy_ctor) {
    Expression fExpression(1), gExpression(2), oExpression(3);

    query.distinct(true);
    query.fetch("name", gExpression);
    query.filter(fExpression);
    query.order_by(oExpression, AttributeQuery::ASC);
    query.limit(10);

    AttributeQuery copy(query);
    
    EXPECT_EQ(fExpression, copy.filter());
    EXPECT_EQ(query.limit(), copy.limit());
    EXPECT_TRUE(copy.distinct());
    AttributeQuery::FetchMap fmap = copy.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(gExpression, fmap["name"]);
    const AttributeQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ(1u, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(AttributeQuery::ASC, ovec.at(0).second);
}

TEST_F(db_AttributeQuery_test, test_copy_assign) {
    Expression fExpression(1), gExpression(2), oExpression(3);
    
    query.distinct(true);
    query.fetch("name", gExpression);
    query.filter(fExpression);
    query.order_by(oExpression, AttributeQuery::ASC);
    query.limit(10);

    AttributeQuery copy;
    copy = query;
    
    EXPECT_EQ(fExpression, copy.filter());
    EXPECT_EQ(query.limit(), copy.limit());
    EXPECT_TRUE(copy.distinct());
    AttributeQuery::FetchMap fmap = copy.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(gExpression, fmap["name"]);
    const AttributeQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ(1u, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(AttributeQuery::ASC, ovec.at(0).second);
}

TEST_F(db_AttributeQuery_test, test_copy_assign_self) {
    Expression fExpression(1), gExpression(2), oExpression(3);

    query.distinct(true);
    query.fetch("name", gExpression);
    query.filter(fExpression);
    query.order_by(oExpression, AttributeQuery::ASC);
    query.limit(10);
    query = query;

    EXPECT_EQ(fExpression, query.filter());
    EXPECT_EQ(10, query.limit());
    EXPECT_TRUE(query.distinct());
    AttributeQuery::FetchMap fmap = query.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(gExpression, fmap["name"]);
    const AttributeQuery::OrderVector& ovec = query.order();
    ASSERT_EQ(1u, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(AttributeQuery::ASC, ovec.at(0).second);
}

TEST_F(db_AttributeQuery_test, test_distinct) {
    query.distinct(true);
    EXPECT_TRUE(query.distinct());
    query.distinct(false);
    EXPECT_FALSE(query.distinct());
}

TEST_F(db_AttributeQuery_test, test_fetch) {
    Expression se(1);

    query.fetch("name", se);
    AttributeQuery::FetchMap fmap = query.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(se, fmap["name"]);
}

TEST_F(db_AttributeQuery_test, test_fetch_duplicate_name) {
    Expression se(1);

    query.fetch("name", se);
    AttributeQuery::FetchMap fmap = query.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(se, fmap["name"]);

    EXPECT_THROW(query.fetch("name", se), duplicate_entry);
    fmap = query.fetch();
    ASSERT_EQ(1u, fmap.size());
    EXPECT_EQ(se, fmap["name"]);
}

TEST_F(db_AttributeQuery_test, test_fetch_empty_name) {
    EXPECT_THROW(query.fetch("", Expression(1)), std::invalid_argument);
}

TEST_F(db_AttributeQuery_test, test_filter) {
    Expression se;

    query.filter(se);
    EXPECT_EQ(se, query.filter());
}

TEST_F(db_AttributeQuery_test, test_order_by) {
    Expression se;

    query.order_by(se, AttributeQuery::ASC);
    const AttributeQuery::OrderVector& ovec = query.order();
    ASSERT_EQ(1u, ovec.size());
    EXPECT_EQ(se, ovec.at(0).first);
    EXPECT_EQ(AttributeQuery::ASC, ovec.at(0).second);
}

TEST_F(db_AttributeQuery_test, test_limit) {
    query.limit(10);
    EXPECT_EQ(10, query.limit());
}

TEST_F(db_AttributeQuery_test, test_group) {
    Expression se;

    query.group(se);
    const AttributeQuery::ExpressionVector& vec = query.group();
    ASSERT_EQ(1u, vec.size());
    EXPECT_EQ(se, vec.at(0));
}

} // namespace brfc
