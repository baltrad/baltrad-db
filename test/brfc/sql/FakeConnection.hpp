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

#ifndef BRFC_SQL_FAKE_CONNECTION_HPP
#define BRFC_SQL_FAKE_CONNECTION_HPP

#include <stdexcept>

#include <brfc/sql/Connection.hpp>

namespace brfc {
namespace sql {

class FakeConnection : public Connection {
  public:
    FakeConnection()
        : open_(false)
        , in_transaction_(false) {
    }

    virtual void do_open() { open_ = true; }

    virtual void do_close() {open_ = false; }

    virtual bool do_is_open() const { return open_; }

    virtual void do_begin() { in_transaction_ = true; }

    virtual void do_commit() { in_transaction_ = false; }

    virtual void do_rollback() { in_transaction_ = false; }

    virtual bool do_in_transaction() const { return in_transaction_; }
    
    virtual Result* do_execute(const std::string&) {
        throw std::runtime_error("not implemented");
    }

    virtual const Dialect& do_dialect() const {
        throw std::runtime_error("not implemented");
    }

    virtual Compiler& do_compiler() {
        throw std::runtime_error("not implemented");
    }

    virtual long long do_store_large_object(const std::string&) {
        throw std::runtime_error("not implemented");
    }

    virtual void do_large_object_to_file(long long, const std::string&) {
        throw std::runtime_error("not implemented");
    }

    virtual long long do_last_insert_id() const {
        throw std::runtime_error("not implemented");
    }
 
  private:
    bool open_;
    bool in_transaction_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_FAKE_CONNECTION_HPP
