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

#ifndef BRFC_SQL_BASIC_CONNECTION_POOL_HPP
#define BRFC_SQL_BASIC_CONNECTION_POOL_HPP

#include <boost/shared_ptr.hpp>

#include <brfc/sql/ConnectionPool.hpp>
#include <brfc/util/Queue.hpp>

namespace brfc {
namespace sql {

class ConnectionCreator;
class ConnectionProxy;
class PoolReturner;

/**
 * @brief a very basic pool with no limits
 */
class BasicConnectionPool : public ConnectionPool {
  public:
    /**
     * @brief constructor
     * @param creator the database connection creator
     * @param max_size maximum number of connections allowed to allocate
     */
    explicit BasicConnectionPool(ConnectionCreator* creator, int max_size=5);
    
    /**
     * @brief destructor
     */
    virtual ~BasicConnectionPool();

    size_t size() const { return size_; }

    size_t max_size() const { return pool_.max_size(); }
    
    /**
     * @brief set a returner
     * @param returner the returner to set (caller retains ownership)
     *
     * this is for testing purposes, already created connections keep the
     * old returner
     */
    void returner(PoolReturner* returner);

  protected:
    /**
     * @brief get a proxied database connection
     * @return ConnectionProxy instance
     *
     * @note ownership of the database connection transfers to the proxy
     */
    virtual Connection* do_get();
    
    /**
     * @brief give a database connection back to the pool
     * @param db the database connection
     *
     * @note ownership of the database connection transfers to this pool
     */
    virtual void do_put(Connection* db);
  
  private:
    /**
     * @brief create, locking size_mutex_
     * @throw db_error if size limit reached
     */
    Connection* create();

    /**
     * @brief delete @c c locking size_mutex_
     */
    void dispose(Connection* conn);

    ConnectionCreator* creator_;
    boost::shared_ptr<PoolReturner> returner_;
    size_t size_; ///< number of allocated connections
    boost::mutex size_mutex_;
    Queue<Connection*> pool_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_BASIC_CONNECTION_POOL_HPP
