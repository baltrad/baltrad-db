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
#ifndef BRFC_SQL_POOL_RETURNER_HPP
#define BRFC_SQL_POOL_RETURNER_HPP

#include <brfc/sql/ConnectionDtor.hpp>

namespace brfc {
namespace sql {

class ConnectionPool;

/**
 * @brief return a connection to pool
 */
class PoolReturner : public ConnectionDtor {
  public:
    /**
     * @brief constructor
     * @param pool the connection pool to return connections to
     */
    PoolReturner(ConnectionPool* pool=0)
            : pool_(pool) {
    }
    
    /**
     * @brief associate with a pool
     */
    void pool(ConnectionPool* pool) {
        pool_ = pool;
    }

    ConnectionPool* pool() const { return pool_; }

  private:
    /**
     * @brief put the connection back to pool or delete the connection
     */
    virtual void do_destroy(Connection* c);
    
    ConnectionPool* pool_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_POOL_RETURNER_HPP
