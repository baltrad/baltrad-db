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

#ifndef BRFC_SQL_CONNECTION_PROXY_HPP
#define BRFC_SQL_CONNECTION_PROXY_HPP

#include <brfc/sql/Connection.hpp>

namespace brfc {
namespace sql {

class ConnectionPool;

/**
 * @brief a proxy for a database connection held by a pool
 */
class ConnectionProxy : public Connection,
                        public enable_shared_from_this<ConnectionProxy> {
  public:    
    static shared_ptr<ConnectionProxy> create(ConnectionPool* pool,
                                              Connection* proxied);

    /**
     * @brief destructor
     *
     * return the connection back to pool
     */
    virtual ~ConnectionProxy();

    Connection* proxied() { return proxied_; }
    
  protected:
    /**
     * @brief constructor
     * @param pool the pool the proxied connection is from
     * @param proxied the proxied database connection
     */
    ConnectionProxy(ConnectionPool* pool, Connection* proxied);

    /**
     * @brief forward the call to the proxied database connection
     */
    virtual void do_open();

    /**
     * @brief forward the call to the proxied database connection
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
    virtual shared_ptr<Result> do_execute(const std::string& stmt);

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
    virtual shared_ptr<LargeObject> do_large_object(long long id);
    
    /**
     * @brief forward the call to the proxied database connection
     */
    virtual shared_ptr<LargeObject> do_large_object(const std::string& path);
    
    /**
     * @brief forward the call to the proxied database connection
     */
    virtual long long do_last_insert_id() const;


  private:
    ConnectionPool* pool_;
    Connection* proxied_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_CONNECTION_PROXY_HPP
