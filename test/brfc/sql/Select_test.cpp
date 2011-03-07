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

#include <brfc/sql/BinaryOperator.hpp>
#include <brfc/sql/Column.hpp>
#include <brfc/sql/Join.hpp>
#include <brfc/sql/Label.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Table.hpp>

namespace {

template<typename T>
bool
contains(const T& container, const typename T::value_type& value) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

} // namespace anonymous

namespace brfc {
namespace sql {

class sql_Select_test : public ::testing::Test {
  public:
    sql_Select_test()
            : t1(Table::create("t1"))
            , t2(Table::create("t2")) {
    }
    
    void SetUp() {
        t1->add_column("c1");
        t1->add_column("c2");
        t2->add_column("d1");
        t2->add_column("d2");
    }

    TablePtr t1, t2;
};

TEST_F(sql_Select_test, test_column) {
    SelectPtr s = Select::create();
    EXPECT_THROW(s->column("c1"), lookup_error);
    s->from(t1);
    s->what(t1->column("c1"));
    
    ColumnPtr c;
    EXPECT_NO_THROW(c = s->column("c1"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), s);
}

TEST_F(sql_Select_test, test_auto_what) {
    SelectPtr s = Select::create(t1->crossjoin(t2));
    EXPECT_TRUE(contains(s->what(), t1->column("c1")));
    EXPECT_TRUE(contains(s->what(), t1->column("c2")));
    EXPECT_TRUE(contains(s->what(), t2->column("d1")));
    EXPECT_TRUE(contains(s->what(), t2->column("d2")));
    EXPECT_TRUE(s->from());
}

TEST_F(sql_Select_test, test_column_labeled_expr) {
    SelectPtr s = Select::create();
    s->what(t1->column("c1")->label("l"));
    s->from(t1);

    EXPECT_THROW(s->column("c1"), lookup_error);

    ColumnPtr c;
    EXPECT_NO_THROW(c = s->column("l"));
    EXPECT_TRUE(c);
    EXPECT_EQ(c->selectable(), s);
}

TEST_F(sql_Select_test, test_matching_column_labeled) {
    SelectPtr s = Select::create();
    s->what(t1->column("c1")->label("l"));
    s->from(t1);

    EXPECT_FALSE(s->matching_column(*t1->column("c2")));
    
    ColumnPtr c = s->matching_column(*t1->column("c1"));
    ASSERT_TRUE(c);
    EXPECT_EQ("l", c->name());
    EXPECT_EQ(s, c->selectable());
}

} // namespace sql
} // namespace brfc
