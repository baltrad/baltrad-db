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
#ifndef BRFC_SQL_PG_CONNECTION_HPP
#define BRFC_SQL_PG_CONNECTION_HPP


#include <brfc/smart_ptr.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/DialectCompiler.hpp>

#include <brfc/sql/pg/Types.hpp>
#include <brfc/sql/pg/Dialect.hpp>

namespace pqxx {
    class connection_base;
    class dbtransaction;
} // namespace pqxx

namespace brfc {

class Url;

namespace sql {
namespace pg {

class Connection : public sql::Connection {
  public:
    explicit Connection(const Url& url);

    explicit Connection(pqxx::connection_base* conn);

    virtual ~Connection();

    static std::string url_to_pg(const Url& url);

    bool has_pqxx_connection() const { return conn_; }
    
    void pqxx_transaction(pqxx::dbtransaction* tx);
  
  protected:
    virtual void do_close();
    virtual bool do_is_open() const;

    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    virtual bool do_in_transaction() const {
        return transaction_.get() != 0;
    }

    virtual const Dialect& do_dialect() const {
        return dialect_;
    }

    virtual Compiler& do_compiler() {
        return compiler_;
    }

    virtual sql::Result* do_execute(const std::string& query);

    virtual long long do_store_large_object(const std::string& path);
    virtual void do_large_object_to_file(long long id, const std::string& path);

    virtual long long do_last_insert_id() const {
        return 0;
    }

  private:
    void load_type_oids();

    Types types_;
    shared_ptr<pqxx::connection_base> conn_;
    shared_ptr<pqxx::dbtransaction> transaction_;
    Dialect dialect_;
    DialectCompiler compiler_;
};

} // namespace pg
} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_PG_CONNECTION_HPP
