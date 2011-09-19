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

#ifndef BRFC_SQL_CONNECTION_PROXY_HPP
#define BRFC_SQL_CONNECTION_PROXY_HPP

#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <brfc/sql/Connection.hpp>

namespace brfc {
namespace sql {

class ResultProxy;

/**
 * @brief a proxy for a database connection held by a pool
 */
class ConnectionProxy : public Connection {
  public:
    typedef boost::function<void(Connection*)> ConnectionDtor;
   
    /**
     * @brief constructor
     * @param proxied the proxied database connection
     * @param conn_dtor destructor used when this proxy is closed
     */
    ConnectionProxy(Connection* proxied, ConnectionDtor conn_dtor);

    /**
     * @brief destructor
     *
     * close, if not closed
     */
    virtual ~ConnectionProxy();

    void connection_dtor(ConnectionDtor dtor) { conn_dtor_ = dtor; }
    ConnectionDtor connection_dtor() const { return conn_dtor_; }

    void remove(ResultProxy* result);

    Connection& proxied();
    const Connection& proxied() const;
    
  protected:
    /**
     * @brief call ConnectionDtor::destroy for the proxied connection
     */
    virtual void do_close();

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual bool do_is_open() const;

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual void do_begin();

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual void do_commit();

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual void do_rollback();

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual bool do_in_transaction() const;

    /**
     * @brief forward the call to the proxied database connection
     * @return the proxied result
     *
     * proxy the result of the call with ResultProxy
     */
    virtual Result* do_execute(const std::string& stmt);

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual const Dialect& do_dialect() const;

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual Compiler& do_compiler();
    
    /**
     * @brief forward the call to the proxied database connection
     */
    virtual long long do_store_large_object(const std::string& path);
    
    /**
     * @brief forward the call to the proxied database connection
     */
    virtual void do_large_object_to_file(long long id, const std::string& path);
    
    /**
     * @brief forward the call to the proxied database connection
     */
    virtual long long do_last_insert_id() const;

  private:

    Connection* proxied_;
    ConnectionDtor conn_dtor_;
    std::list<ResultProxy*> results_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_CONNECTION_PROXY_HPP
