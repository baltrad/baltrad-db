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

#include <list>
#include <map>

#include <boost/function.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <brfc/sql/ConnectionPool.hpp>

namespace brfc {
    class Url;

    namespace sql {
        class ConnectionProxy;
    }
}

namespace brfc {
namespace sql {

/**
 * @brief a very basic pool with no limits
 */
class BasicConnectionPool : public ConnectionPool {
  public:
    typedef boost::function<Connection*()> ConnectionCreator;
    typedef boost::function<void(Connection*)> ConnectionDeleter;
   
    /**
     * @brief construct from url
     *
     * uses DefaultConnectionCreator to create connections and
     * boost::checked_deleter to delete them.
     */
    explicit BasicConnectionPool(const Url& url);

    /**
     * @brief constructor
     * @param conn_creator the database connection creator
     * @param conn_deleter the database connection destructor
     * @param max_size maximum number of connections allowed to allocate
     */
    BasicConnectionPool(ConnectionCreator conn_creator,
                        ConnectionDeleter conn_deleter,
                        int max_size=5);
        
    /**
     * @brief destructor
     */
    virtual ~BasicConnectionPool();

    size_t size() const;

    size_t max_size() const;

    size_t lease_count() const;
  
  private:
    typedef std::map<Connection*, ConnectionProxy*> LeaseMap;

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
  
    /**
     * @brief create, locking size_mutex_
     * @throw db_error if size limit reached
     */
    Connection* create();

    /**
     * @brief delete @c c locking size_mutex_
     */
    void dispose(Connection* conn);

    ConnectionCreator conn_creator_;
    ConnectionDeleter conn_deleter_;
    size_t max_size_;
    mutable boost::recursive_mutex mutex_;
    std::list<Connection*> pool_;
    LeaseMap leased_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_BASIC_CONNECTION_POOL_HPP
