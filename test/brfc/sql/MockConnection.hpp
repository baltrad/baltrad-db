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

#ifndef BRFC_SQL_MOCK_CONNECTION_HPP
#define BRFC_SQL_MOCK_CONNECTION_HPP

#include <gmock/gmock.h>


#include <brfc/smart_ptr.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>

namespace brfc {
namespace sql {

class MockConnection : public Connection {
  public:
    MockConnection(shared_ptr<Compiler> c=shared_ptr<Compiler>())
            : Connection(c) {
    }

    MOCK_METHOD0(do_open, void());
    MOCK_CONST_METHOD0(do_is_open, bool());
    MOCK_METHOD0(do_close, void());

    MOCK_METHOD0(do_begin, void());
    MOCK_METHOD0(do_commit, void());
    MOCK_METHOD0(do_rollback, void());

    MOCK_METHOD1(do_execute, shared_ptr<Result>(const String&));
    MOCK_CONST_METHOD0(do_in_transaction, bool());
    MOCK_CONST_METHOD0(do_dialect, const Dialect&());
    MOCK_METHOD1(do_large_object, shared_ptr<LargeObject>(long long));
    MOCK_METHOD1(do_large_object, shared_ptr<LargeObject>(const String&));
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_MOCK_CONNECTION_HPP
