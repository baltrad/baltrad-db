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

#ifndef BRFC_SQL_BASIC_CONNECTION_POOL_HPP
#define BRFC_SQL_BASIC_CONNECTION_POOL_HPP

#include <boost/thread/mutex.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/sql/ConnectionPool.hpp>

namespace brfc {
namespace sql {

class ConnectionCreator;

/**
 * @brief a very basic pool with no limits
 */
class BasicConnectionPool : public ConnectionPool {
  public:
    /**
     * @brief constructor
     * @param creator the database connection creator
     */
    explicit BasicConnectionPool(shared_ptr<ConnectionCreator> creator);
    
    /**
     * @brief destructor
     */
    virtual ~BasicConnectionPool();

  protected:
    /**
     * @brief get a proxied database connection
     * @return ConnectionProxy instance
     *
     * @note ownership of the database connection transfers to the proxy
     */
    virtual shared_ptr<Connection> do_get();
    
    /**
     * @brief give a database connection back to the pool
     * @param db the database connection
     *
     * @note ownership of the database connection transfers to this pool
     */
    virtual void do_put(Connection* db);
  
  private:
    shared_ptr<ConnectionCreator> creator_;
    boost::mutex lock_;
    boost::ptr_vector<Connection> pool_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_BASIC_CONNECTION_POOL_HPP
