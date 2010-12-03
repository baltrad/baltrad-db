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

#ifndef BRFC_SQL_MOCK_DIALECT_HPP
#define BRFC_SQL_MOCK_DIALECT_HPP

#include <gmock/gmock.h>

#include <brfc/sql/Dialect.hpp>

namespace brfc {
namespace sql {

class MockDialect : public Dialect {
  public:
    MockDialect() {
        ON_CALL(*this, do_variant_to_string(::testing::_))
            .WillByDefault(::testing::Invoke(this, &MockDialect::do_variant_to_string_impl));
    }

    MOCK_CONST_METHOD1(do_has_feature, bool(Dialect::Feature));
    MOCK_CONST_METHOD0(do_name, std::string&());
    MOCK_CONST_METHOD1(do_variant_to_string, std::string(const Variant&));

    std::string do_variant_to_string_impl(const Variant& value) {
        return Dialect::do_variant_to_string(value);
    }
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_MOCK_DIALECT_HPP
