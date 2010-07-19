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

#include <pqxx/connection>
#include <pqxx/transaction>

#include <brfc/smart_ptr.hpp>
#include <brfc/Url.hpp>

#include <brfc/sql/Connection.hpp>

#include <brfc/sql/pg/Types.hpp>

class String;

namespace brfc {
namespace sql {
namespace pg {

class Connection : public sql::Connection {
  public:
    explicit Connection(const Url& url);

    virtual ~Connection();

    static String url_to_pg(const Url& url);
  
  protected:
    virtual void do_open();
    virtual void do_close();
    virtual bool do_is_open() const;

    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    virtual bool do_in_transaction() const {
        return transaction_.get() != 0;
    }

    virtual String do_dialect() const {
        return "postgresql";
    }

    virtual shared_ptr<sql::Result> do_execute(const String& query);

    virtual bool do_has_feature(Feature feature) const;

  private:
    void load_type_oids();

    Url url_;
    Types types_;
    scoped_ptr<pqxx::connection> conn_;
    scoped_ptr<pqxx::transaction<> > transaction_;
};

} // namespace pg
} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_PG_CONNECTION_HPP
