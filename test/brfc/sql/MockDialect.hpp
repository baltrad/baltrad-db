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

#ifndef BRFC_SQL_MOCK_DIALECT_HPP
#define BRFC_SQL_MOCK_DIALECT_HPP

#include <gmock/gmock.h>

#include <brfc/sql/Dialect.hpp>

namespace brfc {
namespace sql {

class MockDialect : public Dialect {
  public:
    MOCK_CONST_METHOD1(do_has_feature, bool(Dialect::Feature));
    MOCK_CONST_METHOD0(do_name, std::string&());
    MOCK_CONST_METHOD1(do_literal_to_string, std::string(const expr::Expression&));
    MOCK_CONST_METHOD1(do_escape, std::string(const std::string&));

    std::string do_literal_to_string_impl(const expr::Expression& value) {
        return Dialect::do_literal_to_string(value);
    }

    void delegate_to_fake() {
        ON_CALL(*this, do_literal_to_string(::testing::_))
            .WillByDefault(::testing::Invoke(this, &MockDialect::do_literal_to_string_impl));
        ON_CALL(*this, do_escape(::testing::_))
            .WillByDefault(::testing::ReturnArg<0>());
    }
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_MOCK_DIALECT_HPP
