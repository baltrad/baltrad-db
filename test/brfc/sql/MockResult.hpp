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

#ifndef BRFC_SQL_MOCK_RESULT_HPP
#define BRFC_SQL_MOCK_RESULT_HPP

#include <gmock/gmock.h>

#include <brfc/sql/Result.hpp>

namespace brfc {

namespace sql {

class MockResult : public Result {
  public:
    MOCK_METHOD0(do_next, bool());
    MOCK_METHOD1(do_seek, bool(int));
    MOCK_METHOD0(do_size, int());

    MOCK_CONST_METHOD1(do_value_at, Variant(unsigned int));
    MOCK_CONST_METHOD1(do_value_at, Variant(const String&));
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_MOCK_RESULT_HPP
