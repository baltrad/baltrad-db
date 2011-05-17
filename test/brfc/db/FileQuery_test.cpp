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

#include <stdexcept>

#include <brfc/db/FileQuery.hpp>

namespace brfc {

class db_FileQuery_test : public ::testing::Test {
  public:
    db_FileQuery_test()
        : query() {
    }
    
    FileQuery query;
};

TEST_F(db_FileQuery_test, test_copy_ctor) {
    Expression fExpression(1), oExpression(2);

    query.filter(fExpression);
    query.order_by(oExpression, FileQuery::ASC);
    query.limit(10);
    query.skip(5);

    FileQuery copy(query);
    
    EXPECT_EQ(fExpression, copy.filter());
    EXPECT_EQ(query.limit(), copy.limit());
    EXPECT_EQ(query.skip(), copy.skip());
    const FileQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(FileQuery::ASC, ovec.at(0).second);
}

TEST_F(db_FileQuery_test, test_copy_assign) {
    Expression fExpression(1), oExpression(2);

    query.filter(fExpression);
    query.order_by(oExpression, FileQuery::ASC);
    query.limit(10);
    query.skip(5);

    FileQuery copy;
    copy = query;
    
    EXPECT_EQ(fExpression, copy.filter());
    EXPECT_EQ(query.limit(), copy.limit());
    EXPECT_EQ(query.skip(), copy.skip());
    const FileQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(FileQuery::ASC, ovec.at(0).second);
}

TEST_F(db_FileQuery_test, test_copy_assign_self) {
    Expression fExpression(1), oExpression(2);

    query.filter(fExpression);
    query.order_by(oExpression, FileQuery::ASC);
    query.limit(10);
    query.skip(5);

    query = query;
    
    EXPECT_EQ(fExpression, query.filter());
    EXPECT_EQ(10, query.limit());
    EXPECT_EQ(5, query.skip());
    const FileQuery::OrderVector& ovec = query.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(oExpression, ovec.at(0).first);
    EXPECT_EQ(FileQuery::ASC, ovec.at(0).second);
}

TEST_F(db_FileQuery_test, test_filter) {
    Expression se(1);

    query.filter(se);
    EXPECT_EQ(se, query.filter());
}

TEST_F(db_FileQuery_test, test_order_by) {
    Expression se(1);

    query.order_by(se, FileQuery::ASC);
    const FileQuery::OrderVector& ovec = query.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(se, ovec.at(0).first);
    EXPECT_EQ(FileQuery::ASC, ovec.at(0).second);
}

TEST_F(db_FileQuery_test, test_limit) {
    query.limit(10);
    EXPECT_EQ(10, query.limit());
}

TEST_F(db_FileQuery_test, test_skip) {
    query.skip(5);
    EXPECT_EQ(5, query.skip());
}

} // namespace brfc
