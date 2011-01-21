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

#include <stdexcept>

#include <brfc/db/FileQuery.hpp>

#include <brfc/db/MockDatabase.hpp>
#include <brfc/db/MockFileResult.hpp>
#include <brfc/expr/MockExpression.hpp>

using ::testing::Matcher;
using ::testing::Ref;
using ::testing::Return;

using brfc::expr::MockExpression;

namespace brfc {
namespace db {

class db_FileQuery_test : public ::testing::Test {
  public:
    db_FileQuery_test()
        : db()
        , query(&db) {
    }
    
    MockDatabase db;
    FileQuery query;
};

TEST_F(db_FileQuery_test, test_copy_ctor) {
    MockExpression fexpr1, fexpr2, fexpr3, oexpr1, oexpr2, oexpr3;

    EXPECT_CALL(fexpr1, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&fexpr2, no_delete)));
    EXPECT_CALL(fexpr2, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&fexpr3, no_delete)));
    EXPECT_CALL(oexpr1, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&oexpr2, no_delete)));
    EXPECT_CALL(oexpr2, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&oexpr3, no_delete)));

    query.filter(fexpr1);
    query.order_by(oexpr1, FileQuery::ASC);
    query.limit(10);

    FileQuery copy(query);
    
    EXPECT_EQ(&fexpr3, copy.filter().get());
    EXPECT_EQ(query.limit(), copy.limit());
    const FileQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(&oexpr3, ovec.at(0).first.get());
}

TEST_F(db_FileQuery_test, test_copy_assign) {
    MockExpression fexpr1, fexpr2, fexpr3, oexpr1, oexpr2, oexpr3;

    EXPECT_CALL(fexpr1, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&fexpr2, no_delete)));
    EXPECT_CALL(fexpr2, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&fexpr3, no_delete)));
    EXPECT_CALL(oexpr1, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&oexpr2, no_delete)));
    EXPECT_CALL(oexpr2, clone())
        .WillOnce(Return(shared_ptr<MockExpression>(&oexpr3, no_delete)));

    query.filter(fexpr1);
    query.order_by(oexpr1, FileQuery::ASC);
    query.limit(10);

    FileQuery copy;
    copy = query;
    
    EXPECT_EQ(&fexpr3, copy.filter().get());
    EXPECT_EQ(query.limit(), copy.limit());
    const FileQuery::OrderVector& ovec = copy.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(&oexpr3, ovec.at(0).first.get());
}

TEST_F(db_FileQuery_test, test_filter) {
    shared_ptr<MockExpression> expr = make_shared<MockExpression>();
    shared_ptr<MockExpression> cexpr = make_shared<MockExpression>();

    EXPECT_CALL(*expr, clone())
        .WillOnce(Return(cexpr));
    
    query.filter(*expr);
    EXPECT_EQ(cexpr, query.filter());
}

TEST_F(db_FileQuery_test, test_order_by) {
    shared_ptr<MockExpression> expr = make_shared<MockExpression>();
    shared_ptr<MockExpression> cexpr = make_shared<MockExpression>();

    EXPECT_CALL(*expr, clone())
        .WillOnce(Return(cexpr));
    
    query.order_by(*expr, FileQuery::ASC);
    const FileQuery::OrderVector& ovec = query.order();
    ASSERT_EQ((size_t)1, ovec.size());
    EXPECT_EQ(cexpr, ovec.at(0).first);
    EXPECT_EQ(FileQuery::ASC, ovec.at(0).second);
}

TEST_F(db_FileQuery_test, test_limit) {
    query.limit(10);
    EXPECT_EQ(10, query.limit());
}

TEST_F(db_FileQuery_test, test_execute) {
    MockFileResult r;
    EXPECT_CALL(db, do_execute(Matcher<const FileQuery&>(Ref(query))))
        .WillOnce(Return(&r));
    
    EXPECT_EQ(&r, query.execute());
}

TEST_F(db_FileQuery_test, test_execute_no_db) {
    FileQuery q;
    EXPECT_THROW(q.execute(), std::runtime_error);
}

} // namespace db
} // namespace brfc
