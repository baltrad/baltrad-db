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

#include <brfc/exceptions.hpp>
#include <brfc/sql/pg/Result.hpp>
#include <brfc/sql/pg/Types.hpp>

#include <pqxx/result>

namespace brfc {
namespace sql {
namespace pg {

class sql_pg_Result_test : public ::testing::Test {
  public:
    sql_pg_Result_test()
            : pqxx_result()
            , types()
            , r(pqxx_result, &types) {
    }

    pqxx::result pqxx_result;
    Types types;
    Result r;
};

TEST_F(sql_pg_Result_test, test_closed_result_throws) {
    r.close();

    EXPECT_THROW(r.next(), db_error);
    EXPECT_THROW(r.seek(1), db_error);
    EXPECT_THROW(r.size(), db_error);
    EXPECT_THROW(r.value_at(0), db_error);
    EXPECT_THROW(r.value_at("column"), db_error);
    EXPECT_THROW(r.affected_rows(), db_error);
}

} // namespace pg
} // namespace sql
} // namespace brfc
