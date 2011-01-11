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
#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>

#include <brfc/sql/FakeConnection.hpp>

namespace brfc {
namespace sql {


class MockConnection : public Connection {
  public:
    MOCK_METHOD0(do_open, void());
    MOCK_CONST_METHOD0(do_is_open, bool());
    MOCK_METHOD0(do_close, void());

    MOCK_METHOD0(do_begin, void());
    MOCK_METHOD0(do_commit, void());
    MOCK_METHOD0(do_rollback, void());

    MOCK_METHOD1(do_execute, Result*(const std::string&));
    MOCK_CONST_METHOD0(do_in_transaction, bool());
    MOCK_CONST_METHOD0(do_dialect, const Dialect&());
    MOCK_METHOD0(do_compiler, Compiler&());
    MOCK_METHOD1(do_large_object, LargeObject*(long long));
    MOCK_METHOD1(do_large_object, LargeObject*(const std::string&));

    MOCK_CONST_METHOD0(do_last_insert_id, long long());

    void delegate_to_fake() {
        using ::testing::Invoke;
        ON_CALL(*this, do_open())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_open));
        ON_CALL(*this, do_close())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_close));
        ON_CALL(*this, do_is_open())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_is_open));
        ON_CALL(*this, do_begin())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_begin));
        ON_CALL(*this, do_commit())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_commit));
        ON_CALL(*this, do_rollback())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_rollback));
        ON_CALL(*this, do_in_transaction())
            .WillByDefault(Invoke(&fake_, &FakeConnection::do_in_transaction));
    }

  private:
    FakeConnection fake_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_MOCK_CONNECTION_HPP
